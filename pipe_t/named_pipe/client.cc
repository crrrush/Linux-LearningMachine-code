#include <iostream>
#include "common.hpp"
#include <cstdio>

using std::cout;
using std::endl;


int main()
{
    int fd = open(PIPE_NAME, O_WRONLY);

    int cnt = 1;
    char buffer[1024];
    while(true)
    { 
        // snprintf(buffer, sizeof(buffer), "i'm client and sending you message. my process pid:%d, count:%d", getpid(), cnt++);
        // if(cnt == 10)
        // {
        //     write(fd, nullptr, 0);
        //     break;
        // }
        fgets(buffer, sizeof(buffer) - 1, stdin);
        if(strlen(buffer) > 0) buffer[strlen(buffer) - 1] = '\0';
        ssize_t n = write(fd, buffer, strlen(buffer));
        assert(strlen(buffer) == n);

    }

    close(fd);

    return 0;
}