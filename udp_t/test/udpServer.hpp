#pragma once

#include<iostream>
#include<string>
#include<cstdio>
#include<cstring>
#include<strings.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


using std::string;
using std::cout;
using std::endl;
using std::cerr;

typedef struct sockaddr_in sockaddr_in;

static const string defaultip = "0.0.0.0";
static const int gnum = 1024;

class udpServer
{

public:
    udpServer(const uint16_t& port, const string& ip = defaultip)
    :_ip(ip), _port(port), _socketfd(-1)
    {
        // 创建socket套接字
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        
        // 绑定端口号和ip
        sockaddr_in sa;
        bzero(&sa, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port); // 输入端口号，并进行网络字节序化
        
        //sa.sin_addr.s_addr = inet_addr(_ip.c_str()); // 输入ip，并进行网络字节序化
        // 服务端不需要绑具体IP地址
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        if(0 != bind(_socketfd, (const struct sockaddr*)&sa, sizeof(sa)))
        {
            cerr << "bind error -> " << errno << " : " << strerror(errno) << endl;
            exit(2);
        }

        // udp服务端初始化完成
    }

    void start()
    {
        char buffer[gnum];
        while(true)
        {
            sockaddr_in sa;
            memset(&sa, 0, sizeof(sa));
            socklen_t len = sizeof(sa);
            ssize_t n = recvfrom(_socketfd, buffer, gnum, 0, (struct sockaddr*)&sa, &len);
            if(n > 0)
            {
                buffer[n] = 0;
                string ip = inet_ntoa(sa.sin_addr);
                uint16_t port = ntohs(sa.sin_port);

                printf("%s[%d]# %s\n", ip.c_str(), port, buffer);
            }
        }
    }

    ~udpServer() { close(_socketfd); }


private:
    string _ip;
    uint16_t _port;
    int _socketfd;
};


