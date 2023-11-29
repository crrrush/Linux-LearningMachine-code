
#include <iostream>
#include <string>
#include <cstdio>

#include "udpServer.hpp"
#include "dict.hpp"
#include <memory>

using namespace std;

void usage()
{
    cout << "\nusage: ./server port\n\n";
}

void dictServer(string &text, const string &index, const string &ip, const uint16_t &port)
{
    printf("[%s][%d]-> word: %s\n", ip.c_str(), port, index.c_str());
    text = dict::GetInstance().find(index);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
        exit(1);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<udpServer> ps(new udpServer(dictServer, port));

    ps->start();

    return 0;
}