#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

template <class T>
class ring_queue
{
#define defaultCap 5
#define defaultPos 0
public:
    ring_queue(size_t cap = defaultCap) : _q(cap), _cap(cap), _rPos(defaultPos), _wPos(defaultPos)
    {
        if (0 != pthread_mutex_init(&_pMutex, nullptr))
        {
            /* error */
        }
        if (0 != pthread_mutex_init(&_vMutex, nullptr))
        {
            /* error */
        }

        if (0 != sem_init(&_write, 0, _cap))
        {
            /* error */
        }
        if (0 != sem_init(&_read, 0, 0))
        {
            /* error */
        }
    }
    ring_queue(const ring_queue &rq) = delete;
    ring_queue &operator=(const ring_queue &rq) = delete;
    ~ring_queue()
    {
        if (0 != pthread_mutex_destroy(&_pMutex))
        {
            /* error */
        }
        if (0 != pthread_mutex_destroy(&_vMutex))
        {
            /* error */
        }

        if (0 != sem_destroy(&_write))
        {
            /* error */
        }
        if (0 != sem_destroy(&_read))
        {
            /* error */
        }
    }

    void push(const T &data)
    {
        P(_write);
        pthread_mutex_lock(&_pMutex);
        _q[_wPos++ % _cap] = data;

        pthread_mutex_unlock(&_pMutex);
        V(_read);
    }

    T &pop()
    {
        P(_read);
        pthread_mutex_lock(&_vMutex);
        T& P = _q[_rPos++ % _cap];

        pthread_mutex_unlock(&_vMutex);
        V(_write);
        return P;
    }

private:
    void P(sem_t &s)
    {
        if (0 != sem_wait(&s))
        {
            /*error*/
        }
    }

    void V(sem_t &s)
    {
        if (0 != sem_post(&s))
        {
            /*error*/
        }
    }

private: /* data */
    std::vector<T> _q;
    size_t _cap;
    sem_t _read;
    sem_t _write;

    size_t _rPos;
    size_t _wPos;

    pthread_mutex_t _pMutex;
    pthread_mutex_t _vMutex;
};
