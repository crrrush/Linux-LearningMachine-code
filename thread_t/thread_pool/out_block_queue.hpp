#pragma once

#include <string>
#include <queue>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <pthread.h>

template <class T>
class out_block_queue
{
public:
    out_block_queue()
    {
        pthread_mutex_init(&_in_mutex, nullptr);
        pthread_mutex_init(&_out_mutex, nullptr);
        pthread_cond_init(&_out_cond, nullptr);
        // execute wrong?  return value? error code?
    }
    ~out_block_queue()
    {
        pthread_mutex_destroy(&_in_mutex);
        pthread_mutex_destroy(&_out_mutex);
        pthread_cond_destroy(&_out_cond);
        // execute wrong?  return value? error code?
    }
    out_block_queue(const out_block_queue &bq) = delete;
    out_block_queue &operator=(const out_block_queue &bq) = delete;

    void push(const T &data)
    {
        pthread_mutex_lock(&_in_mutex);

        // 入队
        _q.push(data);

        // 唤醒在_out_cond条件变量下等待的线程
        pthread_cond_signal(&_out_cond);
        pthread_mutex_unlock(&_in_mutex);
    }

    T &pop()
    {
        pthread_mutex_lock(&_out_mutex);
        // 判断队列中的数据是否等于最大容量
        while (is_empty())
            pthread_cond_wait(&_out_cond, &_out_mutex);

        // 出队
        T &ret = _q.front();
        _q.pop();

        pthread_mutex_unlock(&_out_mutex);

        return ret;
    }

private:
    bool is_empty()
    {
        return _q.empty();
    }

private:
    std::queue<T> _q;

    pthread_mutex_t _in_mutex;
    pthread_mutex_t _out_mutex;
    pthread_cond_t _out_cond;
};
