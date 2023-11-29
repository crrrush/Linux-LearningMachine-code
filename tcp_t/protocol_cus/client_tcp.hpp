#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "log.hpp"
#include "error.hpp"
#include "cal.hpp"
#include "protocol.hpp"

#define BUFFERSIZE 1024

typedef struct sockaddr_in sockaddr_in;

void calclientIO(int _socket)
{
    while (true)
    {
        // 获取发送数据
        // 序列化构建请求
        std::string req = request().serialization(equation(calserver::calculator));

        // 构建报文
        encode(req);

        // 发送数据
        if (-1 == write(_socket, req.c_str(), req.size()))
        {
            // log ERR
            break;
        }

        // 接收应答
        std::string message;
        if(!recvmessage(_socket, &message)) break;

        
        // 解包
        std::string resp = decode(message);
        // 反序列化 构建数据
        result res = response().deserialization(resp);

        res.out();
        usleep(100);
    }
}

class client_tcp
{
public:
    client_tcp(const std::string ip, const uint16_t &port) : _ip(ip), _port(port)
    {
        // 创建tcp套接字
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == _socket)
        {
            // log
            exit(SOCK_ERR);
        }

        // 绑定ip 端口 客户端不需自己显式bind

        // 不需要listen，accept，只需connect
    }

    void run()
    {
        // 填入目标ip，目标port
        sockaddr_in sa;
        socklen_t len = sizeof(sa);
        memset(&sa, 0, len);
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port);
        sa.sin_addr.s_addr = inet_addr(_ip.c_str());

        if (-1 == connect(_socket, (const struct sockaddr *)&sa, len))
        {
            // log WRONG
            exit(CONNECT_ERR);
        }

        calclientIO(_socket);
    }

    // void run()
    // {
    //     // 填入目标ip，目标port
    //     sockaddr_in sa;
    //     socklen_t len = sizeof(sa);
    //     memset(&sa, 0, len);
    //     sa.sin_family = AF_INET;
    //     sa.sin_port = htons(_port);
    //     sa.sin_addr.s_addr = inet_addr(_ip.c_str());

    //     if (-1 == connect(_socket, (const struct sockaddr *)&sa, len))
    //     {
    //         // log WRONG
    //         exit(CONNECT_ERR);
    //     }

    //     std::string s;
    //     while (true)
    //     {
    //         // 获取发送数据
    //         std::getline(std::cin, s);
    //         if(s.size() == 0) s += "\n";

    //         // 发送数据
    //         if (-1 == write(_socket, s.c_str(), s.size()))
    //         {
    //             // log ERR
    //             break;
    //         }

    //         // 接收应答
    //         char buffer[BUFFERSIZE];
    //         ssize_t n = read(_socket, buffer, sizeof(buffer));
    //         if (-1 == n)
    //         {
    //             // log ERROR
    //             close(_socket);
    //             break;
    //         }
    //         buffer[n] = 0;
    //         printf("%s\n", buffer);
    //     }
    // }

    ~client_tcp()
    {
        close(_socket);
    }

private: /* data */
    int _socket;
    std::string _ip;
    uint16_t _port;
};
