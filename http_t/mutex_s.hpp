#pragma once

#include <pthread.h>

class mutex_s
{
public:
    mutex_s()
    {
        // if (0 != pthread_mutex_init(&_mutex, nullptr))
        if (pthread_mutex_init(&_mutex, nullptr))
        {
            // 返回错误码，可记录
            // 退出程序
        }
    }

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    ~mutex_s()
    {
        // if (0 != pthread_mutex_destroy(&_mutex);)
        if (pthread_mutex_destroy(&_mutex))
        {
            // 返回错误码，可记录
            // 退出程序
        }
    }

private:
    pthread_mutex_t _mutex;
};

class lock_guard_s
{
private:
    //mutex_s* _mtx;
    mutex_s& _mtx;

public:
    //lock_guard_s(mutex_s*const & mtx) : _mtx(mtx)
    lock_guard_s(mutex_s& mtx) : _mtx(mtx)
    {
        //_mtx->lock();
        _mtx.lock();
    }
    ~lock_guard_s()
    {
        //_mtx->unlock();
        _mtx.unlock();
    }
};
