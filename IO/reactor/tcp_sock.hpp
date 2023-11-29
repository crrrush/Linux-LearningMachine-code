#pragma once

#include <string>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "error.hpp"
#include "log.hpp"

typedef struct sockaddr_in sockaddr_in;

const static int backlog = 32;

// 创建并使用TCP套接字
class tcp_sock
{
public:
    tcp_sock() : _listensock(-1), _errno(-1) {}
    ~tcp_sock()
    {
        if (_listensock != -1)
            close(_listensock);
    }

public:
    void _socket()
    {
        _listensock = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
        if (-1 == _listensock)
        {
            log(FATAL, "[create socket failed][%d]: %s", errno, strerror(errno));
            exit(SOCK_ERR);
        }

        // 设置端口复用
        int opt = 1;
        setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        log(NORMAL, "create socket successed");
    }

    void _bind(uint16_t port_)
    {
        // 绑定（本主机任意）ip地址和特定端口号
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;         // 协议号 TCP
        sa.sin_port = htons(port_);      // 绑定某一个固定端口
        sa.sin_addr.s_addr = INADDR_ANY; // 本机的任意IP
        if (-1 == bind(_listensock, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            log(FATAL, "[bind port %d failed][%d]: %s", port_, errno, strerror(errno));
            exit(BIND_ERR);
        }
        log(NORMAL, "bind port %d successed", port_);
    }

    void _listen()
    {
        // 设置套接字为监听状态
        if (-1 == listen(_listensock, backlog))
        {
            log(FATAL, "[set listen failed][%d]: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        }
        log(NORMAL, "set listen successed");
    }

    int _accept(std::string *clientip_, uint16_t *clientport_)
    {
        // 从TCP套接字中获取连接
        sockaddr_in link;
        socklen_t len = sizeof(link);
        memset(&link, 0, len);
        int fd = accept(_listensock, (struct sockaddr *)&link, &len);
        if (-1 == fd)
        {
            _errno = errno; // 将错误码带出

            // -1 一定就是有问题吗？
            // 读写条件尚未满足也是返回-1，此时错误码是EAGAIN 或者 EWOULDBLOCK
            // if(errno == EAGAIN)
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                log(WARNING, "[accept conditions were not met][%d]: %s", errno, strerror(errno)); // 读取条件尚不满足
            else if (errno == EINTR)
                log(WARNING, "[accept were interrupted][%d]: %s", errno, strerror(errno)); // 读取过程中被信号中断 退出再读一次
            else
                log(ERROR, "[accept link failed][%d]: %s", errno, strerror(errno));
        }
        else
        {
            *clientip_ = inet_ntoa(link.sin_addr);
            *clientport_ = ntohs(link.sin_port);

            log(NORMAL, "accept link successed");
        }

        return fd;
    }

    int fd() { return _listensock; }

    int accepterr() { return _errno; }

private:
    int _listensock;

    int _errno;
};
