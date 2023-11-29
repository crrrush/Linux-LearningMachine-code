
#include <iostream>
#include <vector>
#include "../thread_class/pthread_s.hpp"
#include "mutex_s.hpp"

using namespace std;

/////////////////////////////////////////////////////使用锁对公共资源设置访问限制而修改数据


int seat = 10000;

mutex_s mtx;


void *func1(void *args)
{
    pthread *msg = (pthread *)args;
    while (seat > 0)
    {
        usleep(1);
        lock_guard_s lock(mtx);
        if (seat > 0)
        {
            cout << msg->_name << " 订位成功 剩余票数：" << seat << endl;
            usleep(10);
            seat--;
        }
    }

    return nullptr;
}

int main()
{
    pthread t1(func1, "thread", 1);
    pthread t2(func1, "thread", 2);
    pthread t3(func1, "thread", 3);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}