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
    int id = shm_create();
    //printf("shmid:%d\n", id);

    createFifo(READPIPE);//创建管道用于读客户端写入完成的信息

    int fd = open(READPIPE, O_RDONLY);

    char* shm = (char*)shm_attach(id);
    //printf("shm address:%p\n", shm);

    //利用管道实现一个互斥锁进行资源保护？
    while(true)
    {
        int n = 0;
        if(read(fd, &n, sizeof(int)) != 4) break;
        if(n == 0) break;
        printf("server recive:#%s\n", shm);
        //fflush(stdout);
        sleep(1);
    }
    
    close(fd);

    delFifo(READPIPE);

    shm_detach(shm);

    shm_delete(id);

    return 0;
}


    // int cnt = 0;
    // while(true)
    // {
    //     cout << ++cnt <<endl;
    //     sleep(1);
    //     if(cnt == 25)
    //         break;
    // }
