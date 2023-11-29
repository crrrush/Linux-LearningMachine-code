#pragma once

#include <vector>
#include <string>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "log.hpp"
#include "error.hpp"
#include "pthread_s.hpp"
#include "protocol_http.hpp"

using func_t = std::function<void (const httpRequest&, httpResponse&)>;

typedef struct sockaddr_in sockaddr_in;

#define DEFAULTIP "0.0.0.0"

#define SEMIJOIN 5

#define BUFFERSIZE 4096

class server_tcp;

struct ts_data
{
public:
    ts_data() {}
    ts_data(server_tcp *ps, int fd) : _ps(ps), _fd(fd) {}

    void operator()();

    server_tcp *_ps;
    int _fd;
};

class server_tcp
{
public:
    server_tcp(const func_t& callback, const uint16_t &port, const std::string &ip = DEFAULTIP) : _socket(-1), _ip(ip), _port(port), _callback(callback)
    {
        // LOG(DEBUG, "constrcutor in");

        // 创建tcp套接字
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == _socket)
        {
            // log
            LOG(FATAL, "socket create failed");
            exit(SOCK_ERR);
        }
        
        // LOG(DEBUG, "socket create successed");

        // 设置端口复用
        int opt = 1;
        setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));

        // 绑定ip 端口
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;         // 协议
        sa.sin_port = htons(_port);      // 固定端口
        sa.sin_addr.s_addr = INADDR_ANY; // 本机的任意ip
        if (-1 == bind(_socket, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            // log
            LOG(FATAL, "bind failed");
            exit(BIND_ERR);
        }

        // LOG(DEBUG, "bind successed");


        // 设置_socket为监听状态
        if (-1 == listen(_socket, SEMIJOIN))
        {
            // log
            exit(LISTEN_ERR);
        }

        // LOG(DEBUG, "constrcutor out");
    }

    ~server_tcp()
    {
        close(_socket);
    }

    void serviceIO(int fd)
    {
        while (true)
        {
            // 读取 ?
            char buffer[BUFFERSIZE];
            // std::cout << "waiting read\n";
            ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
            if (-1 == n)
            {
                // log ERROR
                std::cout << "read error?\n";
                break;
            }
            else if (0 == n)
            {
                // log normal
                std::cout << "client quit\n";
                break;
            }

            // 交付/处理
            buffer[n] = 0; // c式结尾
            httpRequest req;
            httpResponse resp;
            req._inbuffer = buffer;
            req.parse();
            _callback(req, resp);

            // 响应
            if (-1 == write(fd, resp._outbuffer.c_str(), resp._outbuffer.size()))
            {
                // log ERR
                std::cout << "write error?\n";
                break;
            }
        }
        // std::cout << "serviceIO out\n";
    }

    static void *threadservice(void *args)
    {
        pthread_detach(pthread_self());
        thread_data *pd = static_cast<thread_data *>(args);
        ts_data *pts = static_cast<ts_data *>(pd->_arg);

        pts->_ps->serviceIO(pts->_fd);

        close(pts->_fd);
        delete pts;
        std::cout << "thread end\n";
        return nullptr;
    }


    // version3 多线程
    void run()
    {
        while (true)
        {
            // 获取链接 ?
            sockaddr_in link;
            socklen_t len = sizeof(link);
            memset(&link, 0, len);
            // std::cout << "waiting accept\n";
            int fd = accept(_socket, (struct sockaddr *)&link, &len);
            if (-1 == fd)
            {
                // log ERROR
                continue; //?
            }

            // 分配链接服务
            pthread(threadservice, new ts_data(this, fd));

            // std::cout << "分配链接服务完成\n";
        }
    }

private: /* data */
    int _socket;
    std::string _ip;
    uint16_t _port;
    std::vector<int> _v;
    func_t _callback;
};

// ?
void ts_data::operator()()
{
    _ps->serviceIO(_fd);
}