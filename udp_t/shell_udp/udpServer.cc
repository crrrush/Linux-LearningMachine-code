
#include <iostream>
#include <string>
#include <cstdio>

#include "udpServer.hpp"
#include <memory>

using namespace std;

void usage()
{
    cout << "\nusage: ./server port\n\n";
}

void shellServer(string &text, const string &cmd, const string &ip, const uint16_t &port)
{
    printf("[%s][%d]-> command: %s\n", ip.c_str(), port, cmd.c_str());
    FILE* fp = popen(cmd.c_str(), "r");
    if(fp == nullptr)
    {
        text = cmd + " execute error";
    }
    else
    {
        char line[1024];
        while (fgets(line, sizeof(line), fp))
        {
            text += line;
        }
    }

    pclose(fp);
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