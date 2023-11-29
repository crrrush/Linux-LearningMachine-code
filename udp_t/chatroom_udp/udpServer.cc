
#include <iostream>
#include <string>
#include <cstdio>
#include <memory>

#include "udpServer.hpp"
#include "users.hpp"

using namespace std;

void usage()
{
    cout << "\nusage: ./server port\n\n";
}

void shellServer(const string &message, const string &ip, const uint16_t &port, const udpServer *ps)
{
    // 解析 是否在线？是否是上线命令
    printf("[%s][%d]: %s\n", ip.c_str(), port, message.c_str());
    if(message == "offline")
    {
        users::GetInstance().del(ip, port);
        return;
    }
    if (!(users::GetInstance().is_online(ip, port)))
    {
        if (message == "online")
            users::GetInstance().add(ip, port);
        else
            ps->send(string("You are offline.Please input 'online' to enter the chat room!"), ip, port);

        return;
    }
    // printf("[%s][%d] is online\n", ip.c_str(), port);

    // 广播信息
    users::GetInstance().broadcast(message, ip, port, ps);
    // users::GetInstance().broadcast(message, ps);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
        exit(1);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<udpServer> ps(new udpServer(shellServer, port));

    ps->start();

    return 0;
}