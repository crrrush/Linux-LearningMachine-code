#pragma once
#include <iostream>

#include <cstring>
#include <cstdlib>
#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define READPIPE "read_PIPE"


void createFifo(const char* pipename)
{
    int retv = mkfifo(pipename, 0666);
    if(-1 == retv)
    {
        std::cout << "createFifo failed " << errno << " " << strerror(errno) << std::endl;
        exit(1);
    }
}

void delFifo(const char* pipename)
{
    int retv = unlink(pipename);
    if(-1 == retv)
    {
        std::cout << "delFifo failed " << errno << " "<< strerror(errno) << std::endl;
        exit(1);
    }
}

