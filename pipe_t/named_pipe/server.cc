#include <iostream>
#include "common.hpp"

//server.cc
using std::cout;
using std::endl;


int main()
{
    createFifo(PIPE_NAME);

    int fd = open(PIPE_NAME, O_RDONLY);

    while(true)
    {
        char buffer[1024];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        if(n <= 0)
            break;

        buffer[n] = 0;

        cout << buffer << endl;
        //sleep(1);
    }

    close(fd);

    delFifo(PIPE_NAME);

    return 0;
}