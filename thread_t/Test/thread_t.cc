#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <pthread.h>

using namespace std;


//static int cnt = 0;
struct threadmsg
{
    int _num;
    char _name[64];
    pthread_t _tid;
};


void* threadFunc(void* arg)
{
    //sleep(1);
    //pthread_cancel(pthread_self());
    //pthread_detach(pthread_self());
    threadmsg* msg = (threadmsg*)arg;
    int cnt = 0;
    while(true)
    {
        //printf("我是%s，我的pid：%d, cnt：%d\n",name.c_str(), getpid(), ++cnt);
        //printf("我是%s，我的pid：%d, 我的tid:：0x%x\n",name.c_str(), getpid(), pthread_self());
        printf("我是%s，我的pid：%d, 我的tid:：0x%x\n",msg->_name, getpid(), msg->_tid);
        sleep(1);

        if(10 == cnt++) break;

        // if(cnt == 8)
        // {
        //     cout << "新线程要取消自己了！\n";
        //     pthread_cancel(pthread_self());
        // }
    }

    

    return (void*)msg;
}


int main()
{
    vector<threadmsg*> vtmsg;

#define THREADNUM 10
    for(int i = 0;i < THREADNUM;++i)
    {
        //pthread_t tid;
        // char buffer[64];
        // snprintf(buffer, sizeof(buffer), "thread-%d", i + 1);
        //char* buffer = new char[64];
        threadmsg* msg = new threadmsg;
        msg->_num = i + 1;
        snprintf(msg->_name, sizeof(msg->_name), "thread-%d", i + 1);
        pthread_create(&msg->_tid, nullptr, threadFunc, (void*)msg);
        vtmsg.push_back(msg);

        //sleep(1);
    }

    printf("我是主线程，我的pid：%d\n", getpid());
    sleep(1);

    for(const auto& e : vtmsg)
    {
        threadmsg* retval = nullptr;
        int n = pthread_join(e->_tid, (void**)&retval);
        if(0 != n)
        {
            cout << "pthread_join error -> " << n << " : " << strerror(n) << endl;
            exit(1);
        }
        printf("pthread_join sucess. thread id:0X%x, thread number:%d, thread name:%s\n", retval->_tid, retval->_num, retval->_name);
        delete retval;
    }
    //cout << "pthread_join sucess!\n";
    sleep(2);

    // while(true)
    // {
    //     printf("我是主线程，我的pid：%d\n", getpid());
    //     sleep(1);
    // }
    // pthread_t tid;
    // pthread_create(&tid, nullptr, threadFunc, (void*)"新线程");

    //printf("我是主线程，我的pid：%d\n", getpid());

    // while(true)
    // {
    //     printf("我是主线程，我的pid：%d，cnt：%d\n", getpid(), cnt);
    //     // if(cnt == 10)
    //     // {
    //     //     pthread_cancel(tid);
    //     //     cout << "新线程已取消\n";
    //     //     sleep(2);
    //     //     break;
    //     // }
    //     sleep(1);   
    // }


    return 0;
}
