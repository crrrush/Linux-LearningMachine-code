
#include <iostream>
#include <string>
#include <memory>
#include <cstdio>
#include <cstring>

#include "log.hpp"
#include "error.hpp"
#include "protocol_http.hpp"
#include "server_tcp.hpp"

using namespace std;

void usage(const string &exe)
{
    cout << "\r\nusage: " << exe << " port\r\n\r\n";
}



void Get(const httpRequest &req, httpResponse &resp)
{
    std::cout << "----------------------http start----------------------------\n";
    std::cout << req._inbuffer << endl;
    printf("method: %s\n", req._method.c_str());
    printf("url: %s\n", req._url.c_str());
    printf("version: %s\n", req._version.c_str());
    printf("path: %s\n", req._path.c_str());
    printf("suffix: %s\n", req._suffix.c_str());
    std::cout << "-----------------------http end-----------------------------\n";

    std::string line = "HTTP/1.1 200 ok\r\n";
    std::string head = util::sufftodesc(req._suffix);

    std::string body;
    if(!util::loadsource(req._path, body))
    {
        util::loadsource(notfound, body); // 404页面 一定存在
    }

    std::string len = "Content-Length: "; // ?
    len += to_string(body.size());
    len += "\r\n";

    string cookie = "Set-Cookie: num=123456; Max-Age=120\r\n";


    resp._outbuffer += line;
    resp._outbuffer += head;
    resp._outbuffer += cookie;
    resp._outbuffer += len;
    resp._outbuffer += blank;

    resp._outbuffer += body;
}

int main(int argc, char *argv[])
{
    if (2 != argc)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    LOG(DEBUG, "command has been analyzed");
    unique_ptr<server_tcp> ps(new server_tcp(Get, port));

    ps->run();

    return 0;
}