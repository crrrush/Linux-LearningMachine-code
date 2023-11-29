#pragma once

#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

class util
{
public:
    static bool SetNonblock(int fd)
    {
        int fl = fcntl(fd, F_GETFL);
        if (fl < 0)
            return false;

        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        return true;
    }
};