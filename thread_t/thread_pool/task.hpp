#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <ctime>
#include <cassert>
#include <unistd.h>

using func_t = std::function<void()>;
// typedef void (*func_t)();
// typedef std::function<void()> func_t;

using std::cout;
using std::endl;

void GetMessege()
{
    cout << "Getting messege from data base...\n";
}

void RequestShutDown()
{
    cout << "Shuting down link...\n";
}

void Download()
{
    cout << "download...\n";
}

void Upload()
{
    cout << "Upload...\n";
}

void Delete()
{
    cout << "delete data...\n";
}

// 单例模式
class task_pool
{
public:
    static task_pool &get_pool() { return _pool; }

    func_t &operator[](int i) 
    {
        assert(i < size());
        return _vFunc[i]; 
    }

    size_t size() { return _vFunc.size(); }

    ~task_pool() {}

private:
    task_pool()
    {
        _vFunc.push_back(GetMessege);
        _vFunc.push_back(RequestShutDown);
        _vFunc.push_back(Download);
        _vFunc.push_back(Upload);
        _vFunc.push_back(Delete);
    }

    task_pool(const task_pool &pool) = delete;

    task_pool &operator=(const task_pool &pool) = delete;

    /* data */
private:
    std::vector<func_t> _vFunc;

    static task_pool _pool;
};

task_pool task_pool::_pool;
