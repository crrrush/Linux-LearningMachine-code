#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"
#include "sock.hpp"

namespace selectserver
{
#define BUFFERSIZE 1024

    const static int defaultport = 8080;
    const static int fdscap = 1024;
    const static int defaultfd = -1;

    using func_t = std::function<bool(std::string &, std::string &)>;

    class select_server
    {
    private:
        // events 中不止一个文件描述符是就绪的，也可以不仅只有一个事件发生
        // 这里只处理了读事件
        void handle_readevents(fd_set &fds)
        {
            // 检测读事件
            for (int i = 0; i < _rfdarray.size(); ++i)
            {
                if (_rfdarray[i] == defaultfd)
                    continue;

                if (FD_ISSET(_rfdarray[i], &fds) && _rfdarray[i] == _sock)
                    acceptor();
                else if (FD_ISSET(_rfdarray[i], &fds))
                    recver(i);
            }
        }

        void acceptor()
        {
            std::string ip;
            uint16_t port;
            int fd = sock::_accept(_sock, &ip, &port);
            if (-1 == fd)
                return;
            int i = 0;
            for (; i < _rfdarray.size(); ++i) // 找读事件数组空位
            {
                if (_rfdarray[i] != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == _rfdarray.size()) // 没有空位了，服务器已经满载
            {
                log(WARNING, "server had no spare.please wait again!");
                close(fd);
                return;
            }

            _rfdarray[i] = fd; // 将文件fd填入读事件数组  写？异常？
        }

        void recver(size_t pos)
        {
            // 怎么读？ 怎么保证读的报文是正确的，是完整的？ 怎么保证不会读到阻塞？
            // 协议定制，反序列化，网络序列数据转换成结构化数据
            // 这里先直接读取

            // while(true){}
            char buffer[BUFFERSIZE] = {0};
            ssize_t n = read(_rfdarray[pos], buffer, sizeof(buffer) - 1);
            if (-1 == n)
            {
                log(WARNING, "[read error, clent quit][%d]: %s", errno, strerror(errno));
                close(_rfdarray[pos]);
                _rfdarray[pos] = defaultfd; // 将该客户端的文件描述符从读事件数组中移除
                return;
            }
            else if (0 == n)
            {
                log(NORMAL, "client quit"); // 客户端退出
                close(_rfdarray[pos]);
                _rfdarray[pos] = defaultfd; // 将该客户端的文件描述符从读事件数组中移除
                return;
            }

            
            // buffer[n] = 0; // c式结尾
            buffer[n - 1] = '\0'; // c式清除换行符以及加上结尾
            log(DEBUG, "client# %s", buffer);
            std::string req(buffer);

            // 处理数据
            std::string resp;
            if (_callback(req, resp))
            {
                // 数据处理失败/异常？
            }

            // 直接写？ 可能会阻塞
            // 这里先直接写了
            if (-1 == write(_rfdarray[pos], resp.c_str(), resp.size()))
            {
                log(ERROR, "[write error][%d]: %s", errno, strerror(errno));
                return;
            }
        }

    public:
        select_server(func_t cb, uint16_t port = defaultport) : _sock(-1), _port(port), _callback(cb), _rfdarray(fdscap, defaultfd) {}
        ~select_server()
        {
            if (-1 != _sock)
                close(_sock);
        }

        void init()
        {
            _sock = sock::_socket();
            sock::_bind(_sock, _port);
            sock::_listen(_sock);
            _rfdarray[0] = _sock; // 将套接字设置进select读事件管理数组中
        }

        void run()
        {
            fd_set fds;
            while (true)
            {
                FD_ZERO(&fds);
                int maxfd = _rfdarray[0];
                for (const auto &e : _rfdarray)
                {
                    if (defaultfd == e)
                        continue;
                    FD_SET(e, &fds);
                    if (e > maxfd)
                        maxfd = e;
                }

                struct timeval timeout = {1, 0};
                // int n = select(maxfd + 1, &fds, nullptr, nullptr, &timeout);
                int n = select(maxfd + 1, &fds, nullptr, nullptr, nullptr);
                if (n < 0)
                {
                    log(WARNING, "[select events error][%d]: %s", errno, strerror(errno));
                    continue;
                }
                else if (0 == n) // timeout事件未发生
                {
                    log(NORMAL, "timeout...");
                    continue;
                }
                else // handle events
                {
                    handle_readevents(fds);
                }
            }
        }

    private:
        int _sock;
        uint16_t _port;
        func_t _callback;
        std::vector<int> _rfdarray; /*读事件数组*/
        std::vector<int> _wfdarray;  /*写事件数组*/
        // std::vector<int> _efds;  /*异常事件数组*/
    };

}
