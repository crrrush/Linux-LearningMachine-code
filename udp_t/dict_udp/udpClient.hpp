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

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

static const int gnum = 1024;

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
        while (true)
        {
            sockaddr_in sa;
            memset(&sa, 0, sizeof(sa));
            sa.sin_family = AF_INET;
            sa.sin_port = htons(_port);
            sa.sin_addr.s_addr = inet_addr(_ip.c_str());
            cout << "please enter# ";
            cin >> in;

            if (-1 == sendto(_socketfd, in.c_str(), in.size(), 0, (const struct sockaddr *)&sa, sizeof(sa)))
            {
                cerr << "sendto error -> " << errno << " : " << strerror(errno) << endl;
                exit(3); // 3?
            }

            char buffer[gnum];
            socklen_t len = sizeof(sa);
            ssize_t n = recvfrom(_socketfd, buffer, gnum, 0, (struct sockaddr *)&sa, &len);
            if (n > 0)
            {
                buffer[n] = 0; // filter \n
                string ip = inet_ntoa(sa.sin_addr);
                uint16_t port = ntohs(sa.sin_port);

                printf("%s[%d]: respond# %s\n", ip.c_str(), port, buffer);
            }
        }
    }

    ~udpClient() { close(_socketfd); }

private:
    string _ip;
    uint16_t _port;
    int _socketfd;
};
