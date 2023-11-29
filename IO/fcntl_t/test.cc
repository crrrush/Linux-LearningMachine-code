
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include "nonblock_io.hpp"

using namespace std;

void task()
{
    printf("other task beingh processed\n");
}

int main()
{
    SetNonblock(0);
    while(true)
    {
        char buf[1024] = {0};
        ssize_t s = read(0, buf, 1024);
        if(s > 0)
        {
            buf[s - 1] = 0;
            printf("echo# %s\n", buf); // 直接打印
        }
        else if(0 == s)
        {
            printf("read end\n");
            break;
        }
        else
        {
            // s == -1
            // -1 一定就是有问题吗？
            // 读写条件尚未满足也是返回-1，此时错误码是EAGAIN 或者 EWOULDBLOCK
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            // if(errno == EAGAIN)
                task();
            else if(errno == EINTR) // 读取过程中被信号中断
                continue; // 可以有别的处理动作
            else
                std::cout << "read error: " << strerror(errno) << std::endl;
        }

        sleep(1);
    }

    return 0;
}