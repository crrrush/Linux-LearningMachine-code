#pragma once

#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>



void SetNonblock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if(fl < 0)
    {
        // 打印错误信息
        std::cout << "set nonblock error: " << strerror(errno) << std::endl;
        return;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK | fl);
}