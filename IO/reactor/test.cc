
#include <iostream>
#include <memory>

#include "protocol.hpp"
#include "reactor_server.hpp"

#include "dict.hpp"

using namespace std;
using namespace reactorserver;

static void usage(const string &cmd)
{
    cerr << "Usage:\r\n\t" << cmd << " port\r\n\r\n";
}

bool handle_request(const string &req, string &resp)
{
    resp = req;
    return true;
}

bool cal_IO(std::string &in, std::string *out)
{
    std::string package;
    while (recvmessage(in, &package))
    {
        // 构建请求 解包并反序列化 得到结构化数据
        request req;
        equation eq = req.deserialization(decode(package));
        eq.out(); // 打印问题

        // 交付处理
        result res = calserver::calculator(eq);

        // 序列化 构建网络字节流
        std::string resp = response(res).serialization();

        // 打包
        encode(resp);

        // 放到发送缓冲区中
        *out += resp;

        // log(DEBUG, "handle compelete, resp: %s", resp);
    }

    return true;
}

bool dict_handle(std::string &in, std::string *out)
{
    in.pop_back();
    in.pop_back();
    *out = dict::GetInstance().find(in);
    in.clear();
    return true;
}

int main(int argc, char *argv[])
{
    // if (argc != 2)
    // {
    //     usage(argv[0]);
    //     exit(USAGE_ERR);
    // }

    // unique_ptr<server_reactor> svr(new server_reactor(handle_request, atoi(argv[1])));
    // unique_ptr<server_reactor> svr(new server_reactor(cal_IO));
    unique_ptr<server_reactor> svr(new server_reactor(dict_handle));

    svr->init();

    svr->run();

    return 0;
}