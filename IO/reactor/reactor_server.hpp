#pragma once

#include <string>
#include <vector>
#include <functional>

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <ctime>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"
#include "util.hpp"
#include "tcp_sock.hpp"
#include "epoller.hpp"
#include "connection.hpp"

namespace reactorserver
{
#define BUFFERSIZE 1024

    const static int defaultport = 8888;
    const static int eventscap = 1024;
    const static int defaultfd = -1;

    using func_t = std::function<bool(std::string &, std::string *)>;

    class server_reactor;
    struct connection;

    using connptr = connection *;
    using conf_t = std::function<void(connptr)>;

    // 简易保活策略
    struct conntm_t
    {
    public:
        int _longlive;      // 1为长连接 0则不是
        uint64_t _lasttime; // 保活策略
    };

    struct connection
    {
    public:
        connection(int sock, server_reactor *ps) : _sock(sock), _psvr(ps) {}

    public:
        // 注册回调函数
        void reg(conf_t recv, conf_t write, conf_t except)
        {
            _recver = recv;
            _writer = write;
            _excepter = except;
        }

        // 更新时间戳，或者更新长连接设置
        void settime(int live, uint64_t newtime)
        {
            _time._longlive = live;
            _time._lasttime = newtime;
        }

        // 判断连接状态
        bool is_longlive() const { return _time._longlive == 1; }

        // 获取时间戳
        uint64_t gettime() { return _time._lasttime; }

        // 关闭文件描述符
        // void closesock() { close(_sock); }

    public:
        int _sock; // 连接对应的文件描述符/套接字

        std::string _inbuf;  // 读取数据到该缓冲区
        std::string _outbuf; // 写数据到该缓冲区

        conf_t _recver;   // 读方法
        conf_t _writer;   // 写方法
        conf_t _excepter; // 异常处理方法

        server_reactor *_psvr; // server回指指针 ?

        conntm_t _time; // 保活策略
    };

    class conn_manager
    {
    public:
        conn_manager() {}
        ~conn_manager() {}

        conn_manager(const conn_manager&) = delete;
        conn_manager& operator=(const conn_manager&) = delete;

    public:
        typedef std::unordered_map<int, connptr>::iterator conn_iterator;
        typedef std::unordered_map<int, connptr>::const_iterator const_conn_iterator;

        // 创建新连接并添加到管理器中
        connptr newconn(int sock, server_reactor *ps)
        {
            connptr pc = new connection(sock, ps);

            _conns.insert(std::make_pair(sock, pc));
            return pc;
        }

        // 注册对应连接的回调函数
        void reg(int sock, conf_t recv, conf_t write, conf_t except) { _conns[sock]->reg(recv, write, except); }

        // 检测套接字对应连接是否存在
        bool is_alive(int sock) { return _conns.find(sock) != _conns.end(); }

        // 删除连接
        void deleconn(int sock)
        {
            delete _conns[sock];
            _conns[sock] = nullptr;
            _conns.erase(sock);
        }

        // 获取连接
        connptr getconnptr(int sock) { return _conns[sock]; }

        connptr operator[](int sock) { return _conns[sock]; }

        // 连接保活检测
        bool is_timeout(connptr pc, uint64_t limittime)
        {
            if (pc->is_longlive())
                return false;

            if ((uint64_t)time(nullptr) - pc->gettime() > limittime)
                return true;
            else
                return false;
        }

        // 迭代器
        conn_iterator begin() { return _conns.begin(); }
        const_conn_iterator cbegin() const { return _conns.cbegin(); }

        conn_iterator end() { return _conns.end(); }
        const_conn_iterator cend() const { return _conns.cend(); }

    private:
        std::unordered_map<int, connptr> _conns;
    };

    class server_reactor
    {
    private:
        void dispatcher(int n)
        {
            // 遍历就绪事件，并派发
            for (int i = 0; i < n; ++i)
            {
                int fd = _revents[i].data.fd;
                uint32_t event = _revents[i].events;

                if ((event & EPOLLHUP) || (event & EPOLLERR))
                    event |= (EPOLLIN | EPOLLOUT);

                if ((event & EPOLLIN) && _conns.is_alive(fd) && _conns[fd]->_recver)
                    _conns[fd]->_recver(_conns[fd]);
                if ((event & EPOLLOUT) && _conns.is_alive(fd) && _conns[fd]->_writer)
                    _conns[fd]->_writer(_conns[fd]);
            }
        }

        void acceptor(connptr pc)
        {
            while (true)
            {
                std::string ip;
                uint16_t port;
                int fd = _sock._accept(&ip, &port);
                if (-1 == fd)
                {
                    if (_sock.accepterr() == EINTR)
                        continue;
                    else
                        return;
                }

                log(DEBUG, "[%s][%d]", ip.c_str(), port);

                // 设置为非阻塞IO
                util::SetNonblock(fd);

                // 将文件描述符设置进epoll模型中，并设置关心读事件
                _ep.add(fd, EPOLLIN | EPOLLET);

                // 新增连接
                connptr pc = _conns.newconn(fd, this);
                pc->reg(std::bind(&server_reactor::recver, this, std::placeholders::_1), std::bind(&server_reactor::writer, this, std::placeholders::_1), nullptr);

                pc->settime(0, (uint64_t)time(nullptr)); // 设置时间戳和连接状态
            }
        }

        void closefd(connptr pc)
        {
            _ep.del(pc->_sock); // 先将该客户端的事件从epoll模型中移除 删除失败？
            close(pc->_sock);   // 再关闭文件描述符
            // pc->closesock();
            _conns.deleconn(pc->_sock);
        }

        void excepter(connptr pc)
        {
            closefd(pc);
        }

        bool err_IO(int errorno, connptr pc)
        {
            // -1 一定就是有问题吗？
            // 读写条件尚未满足也是返回-1，此时错误码是EAGAIN 或者 EWOULDBLOCK
            // if(errorno == EAGAIN)
            if (errorno == EAGAIN || errorno == EWOULDBLOCK)
            {
                log(NORMAL, "[read and write conditions were not met][%d]: %s", errorno, strerror(errorno));
                return false;
            }
            else if (errorno == EINTR) // 读取过程中被信号中断
            {
                log(NORMAL, "[read or write were interrupted][%d]: %s", errorno, strerror(errorno));
                return true; // 再读一次
            }

            log(WARNING, "[read error,client quit][%d]: %s", errorno, strerror(errorno));
            excepter(pc);
        }

        void recver(connptr pc)
        {
            pc->settime(0, (uint64_t)time(nullptr)); // 更新时间戳
            while (true)
            {
                char buffer[BUFFERSIZE] = {0};
                ssize_t n = read(pc->_sock, buffer, sizeof(buffer));
                if (-1 == n)
                {
                    if (err_IO(errno, pc))
                        continue;

                    return;
                }
                else if (0 == n)
                {
                    log(NORMAL, "client quit"); // 客户端退出
                    closefd(pc);
                    return;
                }

                buffer[n] = 0;
                pc->_inbuf += buffer;

                // 上层从接受缓冲区中读取所有完整的报文，处理完成后将结果放入发送缓冲区
                _callback(pc->_inbuf, &(pc->_outbuf));

                // 由于发送缓冲区大部分情况下是就绪的
                pc->_writer(pc); // 如果一次没发完，再交给epoll关注，直至发完
            }
        }

        void writer(connptr pc)
        {
            while (true)
            {
                ssize_t n = write(pc->_sock, pc->_outbuf.c_str(), pc->_outbuf.size());
                if (-1 == n)
                {
                    if (err_IO(errno, pc))
                        continue;

                    return;
                }
                else if (0 == n)
                {
                    log(NORMAL, "client quit"); // 客户端退出
                    closefd(pc);
                    return;
                }

                pc->_outbuf.erase(0, n);
                if (pc->_outbuf.empty())
                    break;
            }

            if (!pc->_outbuf.empty())
                _ep.mod(pc->_sock, EPOLLIN | EPOLLOUT | EPOLLET);
            else
                _ep.mod(pc->_sock, EPOLLIN | EPOLLET);

            pc->settime(0, (uint64_t)time(nullptr)); // 更新时间戳
        }

    public:
        server_reactor(func_t cb, uint16_t port = defaultport) : _port(port), _revents(nullptr), _callback(cb) {}
        ~server_reactor()
        {
            if (_revents != nullptr)
                delete[] _revents;

            _revents = nullptr;
        }

        void init()
        {
            // 创建epoll模型
            _ep.create();

            // 创建TCP套接字，绑定ip和端口号并设置监听状态
            _sock._socket();
            _sock._bind(_port);
            _sock._listen();

            // 本服务器为ET模式
            // 将TCP套接字对应IO设置为非阻塞式IO
            if (!util::SetNonblock(_sock.fd()))
            {
                log(FATAL, "[TCP sock set nonblock failed][%d]: %s", errno, strerror(errno));
                exit(SOCK_SET_NONBLOCK_ERR);
            }

            // 将套接字设置进epoll模型中，并设置关心读事件
            if (!_ep.add(_sock.fd(), EPOLLIN | EPOLLET))
            {
                log(FATAL, "epoll add sock failed!!!");
                exit(EPOLL_ADD_SOCK_ERR);
            }

            _conns.newconn(_sock.fd(), this);
            _conns[_sock.fd()]->reg(std::bind(&server_reactor::acceptor, this, std::placeholders::_1), nullptr, nullptr);

            _conns[_sock.fd()]->settime(1, 0); // 套接字不能关

            _revents = new epoll_event[eventscap];
        }

        void run()
        {
            // epoll_event* events = new epoll_event[eventscap];
            // epoll_event events[eventscap];
            while (true)
            {
                int n = _ep.wait(_revents, eventscap);
                if (n > 0)
                {
                    dispatcher(n);
                }

                // 保活检测
                for(auto& e : _conns)
                {
                    if(_conns.is_timeout(e.second, 300))
                    {
                        log(DEBUG, "link timout, server shut down this link. fd: %d", e.second->_sock);
                        closefd(e.second);
                    }
                }
            }
        }

    private:
        tcp_sock _sock;
        epoller _ep;
        uint16_t _port;

        epoll_event *_revents; // 就绪事件数组

        func_t _callback; // 回调上层处理业务函数

        conn_manager _conns; // 连接管理器
    };

}
