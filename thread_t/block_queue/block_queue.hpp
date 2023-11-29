#pragma once

#include <string>
#include <queue>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <pthread.h>

template <class T>
class block_queue
{
#define DefaultCap 5
public:
    block_queue(int cap = DefaultCap) : _cap(cap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_in_cond, nullptr);
        pthread_cond_init(&_out_cond, nullptr);
        // execute wrong?  return value? error code?
    }
    ~block_queue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_in_cond);
        pthread_cond_destroy(&_out_cond);
        // execute wrong?  return value? error code?
    }
    block_queue(const block_queue& bq) = delete;
    block_queue& operator=(const block_queue& bq) = delete;

    void push(const T &data)
    {
        pthread_mutex_lock(&_mutex);
        // 判断队列中的数据是否等于最大容量
        while (is_full())
            pthread_cond_wait(&_in_cond, &_mutex);

        // 入队
        _q.push(data);

        // 唤醒在_out_cond条件变量下等待的线程
        pthread_cond_signal(&_out_cond);
        pthread_mutex_unlock(&_mutex);
    }

    T &pop()
    {
        pthread_mutex_lock(&_mutex);
        // 判断队列中的数据是否等于最大容量
        while (is_empty())
            pthread_cond_wait(&_out_cond, &_mutex);

        // 出队
        T &ret = _q.front();
        _q.pop();

        // 唤醒在_in_cond条件变量下等待的线程
        pthread_cond_signal(&_in_cond);
        pthread_mutex_unlock(&_mutex);

        return ret;
    }

private:
    bool is_empty()
    {
        return _q.empty();
    }

    bool is_full()
    {
        return _q.size() == _cap;
    }

private:
    std::queue<T> _q;
    int _cap;

    pthread_mutex_t _mutex;

    pthread_cond_t _in_cond;
    pthread_cond_t _out_cond;
};
