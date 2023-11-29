#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

typedef struct sockaddr_in sockaddr_in;

static const string defaultip = "0.0.0.0";
static const int gnum = 1024;

class udpServer;
using func_t = std::function<void(const std::string &, const std::string &, const uint16_t &, const udpServer *)>;

class udpServer
{
public:
    udpServer(const func_t &cb, const uint16_t &port, const string &ip = defaultip)
        : _callback(cb), _ip(ip), _port(port), _socketfd(-1)
    {
        // 创建socket套接字
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);

        // 绑定端口号和ip
        sockaddr_in sa;
        bzero(&sa, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port); // 输入端口号，并进行网络字节序化

        // sa.sin_addr.s_addr = inet_addr(_ip.c_str()); // 输入ip，并进行网络字节序化
        //  服务端不需要绑具体IP地址
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        if (0 != bind(_socketfd, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            cerr << "bind error -> " << errno << " : " << strerror(errno) << endl;
            exit(2); // 2?
        }

        // udp服务端初始化完成
    }

    void start()
    {
        char buffer[gnum];
        while (true)
        {
            sockaddr_in sa;
            memset(&sa, 0, sizeof(sa));
            socklen_t len = sizeof(sa);
            ssize_t n = recvfrom(_socketfd, buffer, gnum, 0, (struct sockaddr *)&sa, &len);
            if (n > 0)
            {
                buffer[n] = 0; // filter \n
                string ip = inet_ntoa(sa.sin_addr);
                uint16_t port = ntohs(sa.sin_port);

                // printf("%s[%d]# %s\n", ip.c_str(), port, buffer);
                _callback(buffer, ip, port, this);
            }
        }
    }

    void send(const std::string &message, const std::string &ip, const uint16_t &port) const
    {
        // cout <<"send in\n";
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = inet_addr(ip.c_str());

        if (-1 == sendto(_socketfd, message.c_str(), message.size(), 0, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            cerr << "sendto error -> " << errno << " : " << strerror(errno) << endl;
            exit(3); // 3?
        }
        // cout <<"send out\n";
    }

    ~udpServer() { close(_socketfd); }

private:
    func_t _callback;
    string _ip;
    uint16_t _port;
    int _socketfd;
};
