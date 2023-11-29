
#include <iostream>
#include <memory>

#include "client_tcp.hpp"

using namespace std;

void usage(const string& exe)
{
    cout << "\r\nusage: " << exe << "ip port\r\n\r\n";
}


int main(int argc, char* argv[])
{
    if(3 != argc)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    string ip = argv[1];
    uint16_t port = atoi(argv[2]);

    unique_ptr<client_tcp> pc(new client_tcp(ip, port));

    pc->run();

    return 0;
}