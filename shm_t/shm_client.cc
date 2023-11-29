#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "common.hpp"
#include "named_pipe.hpp"

using namespace std;

int main()
{
    // key_t k = ftok(PATHNAME, PROJ_ID);
    // printf("key:%x\n", k);
    int id = shm_get();
    //printf("shmid:%d\n", id);

    char* shm = (char*)shm_attach(id);
    //printf("shm address:%p\n", shm);

    int fd = open(READPIPE, O_WRONLY);

    int flag = 1;
    int cnt = 1;
    const char* msg = "i'm client and sending you message";
    while(true)
    {
        snprintf(shm, MAXSIZE, "%s: pid:%d count:%d", msg, getpid(), cnt);
        if(cnt++ > 12) flag = 0; 
        if((write(fd, &flag, sizeof(int)) != 4) || 0 == flag) break;
        sleep(2);
    }

    close(fd);
    // int cnt = 0;
    // while(true)
    // {
    //     cout << ++cnt <<endl;
    //     sleep(1);
    //     if(cnt == 10)
    //         break;
    // }

    shm_detach(shm);

    return 0;
}