#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <pthread.h>


class pthread
{
public:
    typedef void* (func_t)(void*);
public:
    pthread(func_t& fn, /*void* arg,*/ const char* name = "", int num = 0):_num(num), _name(name), _retval(nullptr)
    {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "-%d", num);
        _name += buffer;

        pthread_create(&_tid, nullptr, fn, (void*)&_num);
    }

    pthread(const pthread&) = delete;

    void join()
    {
        int n = pthread_join(_tid, &_retval);
        if(0 != n)
        {
            std::cout << "pthread_join error -> " << n << " : " << strerror(n) << std::endl;
            exit(1);
        }
    }

//private:
public:
    int _num;
    std::string _name;

    pthread_t _tid;
    void* _retval;
    //func_t _fn;
};
