#pragma once

#include <string>
#include <vector>
#include <functional>

#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"
#include "sock.hpp"

namespace pollserver
{
#define BUFFERSIZE 1024

    const static int defaultport = 8080;
    const static int fdscap = 1024;
    const static int defaultfd = -1;

    using func_t = std::function<bool(const std::string &, std::string &)>;

    class poll_server
    {
    public:
        poll_server(func_t cb, uint16_t port = defaultport) : _sock(-1), _port(port), _callback(cb), _fds(nullptr)
        {
            _fds = new struct pollfd[fdscap];
            for (int i = 0; i < fdscap; ++i)
                resetpollfd(_fds[i]);
        }
        ~poll_server()
        {
            if (-1 != _sock)
                close(_sock);
            if (nullptr != _fds)
                delete[] _fds;

            // _sock = -1;
            // _fds = nullptr;
        }

        void init()
        {
            _sock = sock::_socket();
            sock::_bind(_sock, _port);
            sock::_listen(_sock);

            // 将套接字设置进poll事件数组里，并设置关心读事件
            _fds[0].fd = _sock;
            _fds[0].events = POLLIN;
        }

        void run()
        {
            while (true)
            {
                int n = poll(_fds, fdscap, 2000);
                if (n < 0)
                {
                    log(WARNING, "[poll events error][%d]: %s", errno, strerror(errno));
                    continue;
                }
                else if (0 == n) // timeout事件未发生
                {
                    log(NORMAL, "timeout...");
                    continue;
                }
                else // handle events
                {
                    handle_events();
                }
            }
        }

        // events 中不止一个文件描述符是就绪的，也可以不仅只有一个事件发生
        // 这里只处理了读事件
        void handle_events()
        {
            log(DEBUG, "handle_events in");

            // 检测读事件
            for (int i = 0; i < fdscap; ++i)
            {
                if (_fds[i].fd == defaultfd || !(_fds[i].events & POLLIN)) // 找设置关心读事件的文件描述符
                    continue;

                if (_fds[i].fd == _sock && (_fds[i].revents & POLLIN))
                    acceptor();
                else if (_fds[i].revents & POLLIN)
                    recver(i);
            }

            log(DEBUG, "handle_events out");
        }

        void acceptor()
        {
            log(DEBUG, "acceptor in");

            std::string ip;
            uint16_t port;
            int fd = sock::_accept(_sock, &ip, &port);
            if (-1 == fd)
                return;
            int i = 0;
            for (; i < fdscap; ++i) // 找读事件数组空位
            {
                if (_fds[i].fd != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == fdscap) // 没有空位了，服务器已经满载
            {
                log(WARNING, "server had no spare.please wait again!");
                close(fd);
                return;
            }

            _fds[i].fd = fd;         // 将文件fd填入事件数组
            _fds[i].events = POLLIN; // 设置关心读事件 写？异常？

            log(DEBUG, "acceptor out");
        }

        void resetpollfd(struct pollfd &pfd)
        {
            pfd.fd = defaultfd;
            pfd.events = pfd.revents = 0;
        }

        void recver(size_t pos)
        {
            // 怎么读？ 怎么保证读的报文是正确的，是完整的？ 怎么保证不会读到阻塞？
            // 协议定制，反序列化，网络序列数据转换成结构化数据
            // 这里先直接读取

            // while(true){}
            char buffer[BUFFERSIZE] = {0};
            ssize_t n = read(_fds[pos].fd, buffer, sizeof(buffer) - 1);
            if (-1 == n)
            {
                log(WARNING, "[read error,client quit][%d]: %s", errno, strerror(errno));
                close(_fds[pos].fd);    // 关闭文件描述符
                resetpollfd(_fds[pos]); // 将该客户端的文件描述符从poll数组中移除
                return;
            }
            else if (0 == n)
            {
                log(NORMAL, "client quit"); // 客户端退出
                close(_fds[pos].fd);        // 关闭文件描述符
                resetpollfd(_fds[pos]);     // 将该客户端的文件描述符从poll数组中移除
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
            if (-1 == write(_fds[pos].fd, resp.c_str(), resp.size()))
            {
                log(ERROR, "[write error][%d]: %s", errno, strerror(errno));
                return;
            }
        }

    private:
        int _sock;
        uint16_t _port;
        func_t _callback;
        struct pollfd *_fds;
    };

}
