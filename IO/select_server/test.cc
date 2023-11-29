
#include <iostream>
#include <memory>
#include "select_server.hpp"

#include "dict.hpp"

using namespace std;
using namespace selectserver;

static void usage(const string &cmd)
{
    cerr << "Usage:\r\n\t" << cmd << " port\r\n\r\n";
}

bool handle_request(string& req, string &resp)
{
    req.pop_back();
    resp = dict::GetInstance().find(req);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    unique_ptr<select_server> svr(new select_server(handle_request, atoi(argv[1])));

    svr->init();

    svr->run();

    return 0;
}