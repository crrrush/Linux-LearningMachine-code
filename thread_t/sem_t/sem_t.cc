#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "pthread_s.hpp"

using namespace std;

void *func1(void *args)
{
    thread_data *pd = static_cast<thread_data *>(args);
    sem_t &s = *(static_cast<sem_t *>(pd->_arg));

    sleep(5);
    for (int i = 0; i < 5; ++i)
    {
        cout << "发布信号量次数：" << i + 1 << endl;
        cout << "-------------------\n";
        sem_post(&s);
        sleep(2);
    }

    return nullptr;
}

void *func2(void *args)
{
    thread_data *pd = static_cast<thread_data *>(args);
    sem_t &s = *(static_cast<sem_t *>(pd->_arg));

    for (int i = 0; i < 5; ++i)
    {
        cout << "获取信号量 次数：" << i + 1 << endl;
        cout << "-------------------\n";
        sem_wait(&s);
    }

    return nullptr;
}

int main()
{
    sem_t s;
    if (-1 == sem_init(&s, 0, 0))
        exit(1);

    pthread t1(func1, &s);
    pthread t2(func2, &s);

    t1.join();
    t2.join();

    if (-1 == sem_destroy(&s))
        exit(1);

    return 0;
}
