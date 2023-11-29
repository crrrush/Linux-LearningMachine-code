#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>



#define PATHNAME "./"
#define PROJ_ID 0x1919

#define MAXSIZE 4096

int ShmGet(int flag)
{
    key_t k = ftok(PATHNAME, PROJ_ID);
    int id = shmget(k, MAXSIZE, flag);
    if(-1 == id)
    {
        std::cout << "shmget error->" << errno << ": " << strerror(errno) << std::endl;
        exit(errno);
    }
    return id;
}

int shm_create()
{
    return ShmGet(IPC_CREAT | IPC_EXCL | 0666);
}

int shm_get()
{
    return ShmGet(IPC_CREAT);
}

void* shm_attach(int shmid)
{
    void* p = shmat(shmid, nullptr, 0666);
    if(-1 == (long)p)
    {
        std::cout << "shm_attach error->" << errno << ": " << strerror(errno) << std::endl;
        exit(errno);
    }
    return p;
}

void shm_detach(const void* shmaddr)
{
    if(-1 == shmdt(shmaddr))
    {
        std::cout << "shm_detach error->" << errno << ": " << strerror(errno) << std::endl;
        exit(errno);
    }
}


void shm_delete(int shmid)
{
    if(-1 == shmctl(shmid, IPC_RMID, nullptr))
    {
        std::cout << "shm_delete error->" << errno << ": " << strerror(errno) << std::endl;
        exit(errno);
    }
    std::cout<<"delete sucess"<<std::endl;
}


#endif