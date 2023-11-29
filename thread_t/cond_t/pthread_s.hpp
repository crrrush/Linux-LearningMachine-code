#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <pthread.h>

struct thread_data
{
    thread_data() : _arg(nullptr), _num(0) {}

    thread_data(void *arg, std::string name, int num) : _arg(arg), _num(num), _name(name) {}

    void *_arg;
    int _num;
    std::string _name;
};

class pthread
{
public:
    typedef void *(func_t)(void *);

public:
    pthread(func_t &fn, void *arg = nullptr, const char *name = "", int num = -1) : _data(arg, name, num), _retval(nullptr)
    {
        if (num != -1)
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "-%d", num);
            _data._name += buffer;
        }

        pthread_create(&_tid, nullptr, fn, (void *)&_data);
    }

    pthread(const pthread &) = delete;

    void join()
    {
        int n = pthread_join(_tid, &_retval);
        if (0 != n)
        {
            std::cout << "pthread_join error -> " << n << " : " << strerror(n) << std::endl;
            exit(1);
        }
    }


public:
    thread_data _data;

private:
    pthread_t _tid;
    void *_retval;
    // func_t _fn;
};
