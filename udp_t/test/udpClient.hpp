#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::cout;
using std::endl;
using std::cerr;
using std::cin;
using std::string;

typedef struct sockaddr_in sockaddr_in;

class udpClient
{
public:
    udpClient(const uint16_t &port, const string &ip)
        : _ip(ip), _port(port)
    {
        // 创建socket套接字
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);

        // udp客户端初始化完成
    }

    void run()
    {
        string in;
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port);
        sa.sin_addr.s_addr = inet_addr(_ip.c_str());
        while (true)
        {
            cout << "please enter# ";
            cin >> in;

            if(-1 == sendto(_socketfd, in.c_str(), in.size(), 0, (const struct sockaddr *)&sa, sizeof(sa)))
            {
                cerr << "sendto error -> " << errno << " : " << strerror(errno) << endl;
                exit(3);
            }

        }
    }

    ~udpClient() {close(_socketfd);}

private:
    string _ip;
    uint16_t _port;
    int _socketfd;
};
