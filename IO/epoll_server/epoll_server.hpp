#pragma once

#include <string>
#include <vector>
#include <functional>

#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"
#include "sock.hpp"
#include "epoll.hpp"

namespace epollserver
{
#define BUFFERSIZE 1024

    const static int defaultport = 8080;
    const static int eventscap = 1024;
    const static int defaultfd = -1;

    using func_t = std::function<bool(const std::string &, std::string &)>;

    class epoll_server
    {
    public:
        epoll_server(func_t cb, uint16_t port = defaultport) : _sock(-1), _epfd(-1), _port(port), _callback(cb) {}
        ~epoll_server()
        {
            if (-1 != _sock)
                close(_sock);
            if (-1 != _epfd)
                close(_epfd);

            // _sock = -1;
            // _epfd = -1;
        }

        void init()
        {
            _epfd = epoll::create();
            _sock = sock::_socket();
            sock::_bind(_sock, _port);
            sock::_listen(_sock);

            // 将套接字设置进epoll模型中，并设置关心读事件
            epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _sock;
            epoll::add(_epfd, _sock, &ev);
        }

        void run()
        {
            // epoll_event* events = new epoll_event[eventscap];
            epoll_event events[eventscap];
            while (true)
            {
                int n = epoll::time_wait(_epfd, events, eventscap, 2000);
                if (n > 0)
                {
                    handle_events(events, n);
                }
            }
        }

        // events 中不止一个文件描述符是就绪的，也可以不仅只有一个事件发生
        // 这里只处理了读事件
        void handle_events(epoll_event *events, int n)
        {
            // log(DEBUG, "handle_events in");

            // 检测读事件
            for (int i = 0; i < n; ++i)
            {
                if (events[i].data.fd == _sock && (events[i].events & EPOLLIN))
                    acceptor();
                else if (events[i].events & EPOLLIN)
                    recver(events, i);
            }

            // log(DEBUG, "handle_events out");
        }

        void acceptor()
        {
            // log(DEBUG, "acceptor in");

            std::string ip;
            uint16_t port;
            int fd = sock::_accept(_sock, &ip, &port);
            if (-1 == fd)
                return;

            // 将文件描述符设置进epoll模型中，并设置关心读事件
            epoll_event ev;
            ev.data.fd = fd;     // 将文件fd填入事件数组
            ev.events = EPOLLIN; // 设置关心读事件 写？异常？
            epoll::add(_epfd, fd, &ev);

            // log(DEBUG, "acceptor out");
        }

        void recver(epoll_event *events, int pos)
        {
            // 怎么读？ 怎么保证读的报文是正确的，是完整的？ 循环读取，什么时候跳出循环，怎么保证不会读到阻塞？
            // 协议定制，反序列化，网络序列数据转换成结构化数据
            // 这里先直接读取

            // while(true){}
            char buffer[BUFFERSIZE] = {0};
            ssize_t n = read(events[pos].data.fd, buffer, sizeof(buffer) - 1);
            if (-1 == n)
            {
                log(WARNING, "[read error,client quit][%d]: %s", errno, strerror(errno));
                epoll::del(_epfd, events[pos].data.fd); // 先将该客户端的事件从epoll模型中移除 删除失败？
                close(events[pos].data.fd);                           // 再关闭文件描述符
                return;
            }
            else if (0 == n)
            {
                log(NORMAL, "client quit");                           // 客户端退出
                epoll::del(_epfd, events[pos].data.fd); // 先将该客户端的事件从epoll模型中移除 删除失败？
                close(events[pos].data.fd);                           // 再关闭文件描述符
                return;
            }
            // buffer[n] = 0; // c式结尾
            buffer[n - 1] = 0; // c式清除换行符以及加上结尾
            log(NORMAL, "client# %s", buffer);

            // 处理数据
            std::string resp;
            if (_callback(buffer, resp))
            {
                // 数据处理失败/异常？
            }

            // 直接写？ 可能会阻塞
            // 这里先直接写了
            if (-1 == write(events[pos].data.fd, resp.c_str(), resp.size()))
            {
                log(ERROR, "[write error][%d]: %s", errno, strerror(errno));
                return;
            }
        }

    private:
        int _sock;
        int _epfd;
        uint16_t _port;
        func_t _callback;
    };

}
