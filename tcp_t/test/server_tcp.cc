
#include <iostream>
#include <string>
#include <memory>

#include "log.hpp"
#include "error.hpp"

#include "server_tcp.hpp"

using namespace std;

void usage(const string& exe)
{
    cout << "\r\nusage: " << exe << " port\r\n\r\n";
}

int main(int argc, char* argv[])
{
    if(2 != argc)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<server_tcp> ps(new server_tcp(port));

    ps->run();


    return 0;
}