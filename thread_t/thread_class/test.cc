#include <iostream>
#include <vector>
#include "pthread_s.hpp"

using namespace std;

/////////////////////////////////////////////////////使用锁对公共资源设置访问限制而修改数据

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int seat = 10000;

void *func1(void *args)
{
    pthread *msg = (pthread *)args;
    while (seat > 0)
    {
        usleep(1);
        pthread_mutex_lock(&mutex);
        if (seat > 0)
        {
            cout << msg->_name << " 订位成功 剩余票数：" << seat << endl;
            usleep(10);
            seat--;
        }
        pthread_mutex_unlock(&mutex);
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

// /////////////////////////////////////////////////////不对公共资源设置访问限制而修改数据

// int seat = 10000;

// void* func1(void* args)
// {
//     pthread* msg = (pthread*)args;
//     while(seat > 0)
//     {
//         usleep(1);
//         cout << msg->_name << " 订位成功 剩余票数：" << seat << endl;
//         usleep(10);
//         seat--;
//     }

//     return nullptr;
// }

// int main()
// {
//     pthread t1(func1, "thread", 1);
//     pthread t2(func1, "thread", 2);
//     pthread t3(func1, "thread", 3);

//     t1.join();
//     t2.join();
//     t3.join();

//     return 0;
// }

//////////////////////////////////////////// 创建一组线程

// void* threadFunc(void* arg)
// {
//     //sleep(1);
//     //pthread_cancel(pthread_self());
//     //pthread_detach(pthread_self());
//     pthread* msg = (pthread*)arg;
//     int cnt = 0;
//     while(true)
//     {
//         printf("我是%s，我的pid：%d, 我的tid:：0x%x\n",msg->_name.c_str(), getpid(), msg->_tid);
//         sleep(1);

//         if(2 == cnt++) break;
//     }

//     msg->_name = "dead thead";
//     msg->_num = 0;

//     return (void*)msg;
// }

// int main()
// {
//     vector<pthread*> vtmsg;

// #define THREADNUM 10
//     for(int i = 0;i < THREADNUM;++i)
//     {
//         pthread* msg = new pthread(threadFunc, "thread", i + 1);
//         vtmsg.push_back(msg);
//     }

//     printf("我是主线程，我的pid：%d\n", getpid());
//     sleep(1);

//     for(const auto& e : vtmsg)
//     {
//         e->join();
//         pthread* retval = (pthread*)e->_retval;
//         printf("pthread_join sucess. thread id:0X%x, thread number:%d, thread name:%s\n", retval->_tid, retval->_num, retval->_name.c_str());
//         //delete e;
//         delete retval;
//     }
//     cout << "pthread_join all sucess!\n";
//     sleep(2);

//     return 0;
// }

//////////////////////////////////////////// 创建线程

// void* func1(void* args)
// {
//     pthread* msg = (pthread*)args;
//     int cnt = 0;
//     while(true)
//     {
//         cout << msg->_name << " is processing! cnt: " << cnt << endl;
//         if(8 == cnt++) break;

//         sleep(1);
//     }

//     return nullptr;
// }

// int main()
// {
//     pthread t1(func1, "thread", 1);

//     t1.join();
//     sleep(2);
//     cout << "join sucess!\n";

//     // while(true)
//     // {
//     //     cout << "main thread is processing!\n";
//     //     sleep(1);
//     // }

//     return 0;
// }
