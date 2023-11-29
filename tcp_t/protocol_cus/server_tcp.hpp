#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "log.hpp"
#include "error.hpp"
#include "pthread_s.hpp"
#include "thread_pool.hpp"
#include "cal.hpp"
#include "protocol.hpp"

typedef struct sockaddr_in sockaddr_in;

#define DEFAULTIP "0.0.0.0"

#define SEMIJOIN 5

#define BUFFERSIZE 1024

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

void recycleChild(int signo)
{
    // std::cout << "waiting child process" << std::endl;
    pid_t id = wait(nullptr);
    std::cout << "wait sucess, pid: " << id << std::endl;
}

void calserverIO(int fd)
{
    while (true)
    {
        // 读取/获取 ?
        std::string message;
        if(!recvmessage(fd, &message)) break;
        // std::cout << message << std::endl;

        // 解包
        // 反序列化 构建数据
        request req;
        equation eq = req.deserialization(decode(message));
        eq.out();

        // 交付/处理
        result res = req.cal();

        // 序列化 构建网络字节流
        std::string resp = response(res).serialization();

        // 构建报文
        encode(resp);

        // 响应

        // 直接发？
        if (-1 == write(fd, resp.c_str(), resp.size()))
        {
            // log ERR
            std::cout << "write error?\n";
            break;
        }

        sleep(1);
    }
}

class server_tcp
{
public:
    server_tcp(const uint16_t &port, const std::string &ip = DEFAULTIP) : _socket(-1), _ip(ip), _port(port)
    {
        // 创建tcp套接字
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == _socket)
        {
            // log
            exit(SOCK_ERR);
        }

        // 绑定ip 端口
        sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;         // 协议
        sa.sin_port = htons(_port);      // 固定端口
        sa.sin_addr.s_addr = INADDR_ANY; // 本机的任意ip
        if (-1 == bind(_socket, (const struct sockaddr *)&sa, sizeof(sa)))
        {
            // log
            exit(BIND_ERR);
        }

        // 设置_socket为监听状态
        if (-1 == listen(_socket, SEMIJOIN))
        {
            // log
            exit(LISTEN_ERR);
        }
    }

    ~server_tcp()
    {
        close(_socket);
    }

    void serviceIO(int fd)
    {
        while (true)
        {
            // 读取/获取 ?
            char buffer[BUFFERSIZE];
            // std::cout << "waiting read\n";
            ssize_t n = read(fd, buffer, sizeof(buffer));
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
            if (n > 0)
            {
                buffer[n] = 0;
                printf("%s\n", buffer);
            }

            // 响应
            static const std::string res("receive message sucess");
            // std::cout << "waiting write\n";
            if (-1 == write(fd, res.c_str(), res.size()))
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

        // pts->_ps->serviceIO(pts->_fd);
        calserverIO(pts->_fd);

        close(pts->_fd);
        delete pts;
        std::cout << "thread end\n";
        return nullptr;
    }

    // // version4 线程池
    // void run()
    // {
    //     thread_pool<ts_data>::GetInstance()->run();
    //     while (true)
    //     {
    //         // 获取链接 ?
    //         sockaddr_in link;
    //         socklen_t len = sizeof(link);
    //         memset(&link, 0, len);
    //         // std::cout << "waiting accept\n";
    //         int fd = accept(_socket, (struct sockaddr *)&link, &len);
    //         if (-1 == fd)
    //         {
    //             // log ERROR
    //             continue; //?
    //         }

    //         // 分配链接服务
    //         thread_pool<ts_data>::GetInstance()->push(ts_data(this, fd));

    //         // std::cout << "分配链接服务完成\n";
    //     }
    // }

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
            pthread t(threadservice, new ts_data(this, fd));

            // std::cout << "分配链接服务完成\n";
        }
    }

    // // version2.2 多进程 SIGCHILD回收子进程
    // void run()
    // {
    //     signal(SIGCHLD, recycleChild);
    //     while (true)
    //     {
    //         // 获取链接 ?
    //         sockaddr_in link;
    //         socklen_t len = sizeof(link);
    //         memset(&link, 0, len);
    //         int fd = accept(_socket, (struct sockaddr *)&link, &len);
    //         if (-1 == fd)
    //         {
    //             // log ERROR
    //             continue; //?
    //         }

    //         // 分配链接服务
    //         pid_t id = fork();
    //         if (0 == id) // child
    //         {
    //             close(_socket); // 关闭套接字
    //             // if(fork() > 0) exit(0);
    //             serviceIO(fd);
    //             close(fd);
    //             exit(0);
    //         }
    //         close(fd);
    //     }
    // }

    // // version2.1 多进程
    // void run()
    // {
    //     while (true)
    //     {
    //         // 获取链接 ?
    //         sockaddr_in link;
    //         socklen_t len = sizeof(link);
    //         memset(&link, 0, len);
    //         int fd = accept(_socket, (struct sockaddr *)&link, &len);
    //         if (-1 == fd)
    //         {
    //             // log ERROR
    //             continue; //?
    //         }

    //         // 分配链接服务
    //         pid_t id = fork();
    //         if (0 == id) // child
    //         {
    //             close(_socket); // 关闭套接字
    //             // if(fork() > 0) exit(0);
    //             serviceIO(fd);
    //             close(fd);
    //             exit(0);
    //         }
    //         close(fd);
    //         // wait(nullptr);

    //         if(waitpid(id, nullptr, 0) > 0)
    //         {
    //             // log
    //             std::cout << "wait sucess\n";
    //         }
    //     }
    // }

    // // version1 阻塞式服务单链接
    // void run()
    // {
    //     while (true)
    //     {
    //         // 获取链接 ?
    //         sockaddr_in link;
    //         socklen_t len = sizeof(link);
    //         memset(&link, 0, len);
    //         int fd = accept(_socket, (struct sockaddr *)&link, &len);
    //         if (-1 == fd)
    //         {
    //             // log ERROR
    //             return; //?
    //         }

    //         serviceIO(fd);
    //         close(fd);
    //     }
    // }

private: /* data */
    int _socket;
    std::string _ip;
    uint16_t _port;
    std::vector<int> _v;
};

// ?
void ts_data::operator()()
{
    _ps->serviceIO(_fd);
}