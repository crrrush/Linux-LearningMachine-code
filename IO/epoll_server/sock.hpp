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

// 创建并使用TCP套接字
class sock
{
public:
    static int _socket()
    {
        int tcpsock = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
        if (-1 == tcpsock)
        {
            log(FATAL, "[create socket failed][%d]: %s", errno, strerror(errno));
            exit(SOCK_ERR);
        }

        // 设置端口复用
        int opt = 1;
        setsockopt(tcpsock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        log(NORMAL, "create socket successed");
        return tcpsock;
    }

    static void _bind(int sock_, uint16_t port_)
    {
        // 绑定（本主机任意）ip地址和特定端口号
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;         // 协议号 TCP
        sa.sin_port = htons(port_);      // 绑定某一个固定端口
        sa.sin_addr.s_addr = INADDR_ANY; // 本机的任意IP
        if (-1 == bind(sock_, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            log(FATAL, "[bind port %d failed][%d]: %s", port_, errno, strerror(errno));
            exit(BIND_ERR);
        }
        log(NORMAL, "bind port %d successed", port_);
    }

    static void _listen(int sock_)
    {
        // 设置套接字为监听状态
        if (-1 == listen(sock_, backlog))
        {
            log(FATAL, "[set listen failed][%d]: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        }
        log(NORMAL, "set listen successed");
    }

    static int _accept(int sock_, std::string *clientip_, uint16_t *clientport_)
    {
        // 从TCP套接字中获取连接
        sockaddr_in link;
        socklen_t len = sizeof(link);
        memset(&link, 0, len);
        int fd = accept(sock_, (struct sockaddr *)&link, &len);
        if (-1 == fd)
        {
            log(WARNING, "[accept link error][%d]: %s", errno, strerror(errno));
            return -1;
        }

        *clientip_ = inet_ntoa(link.sin_addr);
        *clientport_ = ntohs(link.sin_port);

        log(NORMAL, "accept link successed");
        return fd;
    }

private:
    const static int backlog = 32;
};
