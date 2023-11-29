
#include <iostream>
#include <memory>
#include "poll_server.hpp"

using namespace std;
using namespace pollserver;

static void usage(const string &cmd)
{
    cerr << "Usage:\r\n\t" << cmd << " port\r\n\r\n";
}

bool handle_request(const string &req, string &resp)
{
    resp = req;
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    unique_ptr<poll_server> svr(new poll_server(handle_request, atoi(argv[1])));

    svr->init();

    svr->run();

    return 0;
}