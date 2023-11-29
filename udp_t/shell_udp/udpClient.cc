
#include "udpClient.hpp"
#include <memory>

using namespace std;

void usage()
{
    cout << "\nusage: ./server ip port\n\n";
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usage();
        exit(1);
    }
    string ip = argv[1];
    uint16_t port = atoi(argv[2]);

    unique_ptr<udpClient> pc(new udpClient(port, ip));

    pc->run();

    return 0;
}