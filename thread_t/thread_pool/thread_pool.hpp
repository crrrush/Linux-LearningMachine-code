#pragma once

#include <vector>
// #include <functional>
#include <memory>
#include "mutex_s.hpp"
#include "pthread_s.hpp"
#include "out_block_queue.hpp"

template <class T, size_t N>
class thread_pool;

template <class T, size_t N = 3>
class thread_pool
{
private:
    static void *routine(void *args)
    {
        thread_data *pd = static_cast<thread_data *>(args);
        out_block_queue<T> *p = static_cast<out_block_queue<T> *>(pd->_arg);
        while (true)
        {
            T t = p->pop();

            cout << "get task sucess , execute the task -> ";
            t();

            // sleep(4);
        }
    }

public:
    ~thread_pool()
    {
        for (auto &e : _vp)
        {
            e->join();
        }
    }

    void run()
    {
        for (auto &e : _vp)
        {
            e = std::unique_ptr<pthread>(new pthread(routine, &_bq));
        }
    }

    void push(const T &t)
    {
        _bq.push(t);
    }

    static std::unique_ptr<thread_pool<T, N>> &GetInstance()
    {
        if (!Tpoll)
        {
            lock_guard_s guard(_InstanceMutex);
            if (!Tpoll)
                Tpoll = std::unique_ptr<thread_pool<T, N>>(new thread_pool<T, N>);
        }

        return Tpoll;
    }

private:
    thread_pool(const thread_pool &pool) = delete;
    thread_pool &operator=(const thread_pool &pool) = delete;
    thread_pool(const size_t n = N) : _vp(n) {}

private: // data
    // size_t _num;
    out_block_queue<T> _bq;
    std::vector<std::unique_ptr<pthread>> _vp;

    static std::unique_ptr<thread_pool<T, N>> Tpoll;
    static mutex_s _InstanceMutex;
};

template <class T, size_t N>
std::unique_ptr<thread_pool<T, N>> thread_pool<T, N>::Tpoll;

template <class T, size_t N>
mutex_s thread_pool<T, N>::_InstanceMutex;
