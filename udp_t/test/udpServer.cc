
#include "udpServer.hpp"
#include <memory>

using namespace std;

void usage()
{
    cout << "\nusage: ./server port\n\n";
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage();
        exit(1);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<udpServer> ps(new udpServer(port));

    ps->start();


    return 0;
}