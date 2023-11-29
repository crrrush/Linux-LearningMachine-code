#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#define PROCESS_NUM 5

class proc_info
{
public:
    proc_info(pid_t pid, int wfd,int i)
    :_pid(pid),_wfd(wfd)
    {
        char a[64];
        sprintf(a, "procmap[%d] -> process id:%d", i, _pid);
        _name = a;
    }
    

public:
    pid_t _pid;
    int _wfd;
    string _name;
};


////////////////////////////////////////////////////// 任务区

typedef void(*func_t)(); //任务函数指针封装

void TaskFunc1()
{
    cout << "Task1" << endl;
    sleep(1);
}

void TaskFunc2()
{
    cout << "Task2" << endl;
    sleep(1);
}

void TaskFunc3()
{
    cout << "Task3" << endl;
    sleep(1);
}

void TaskFunc4()
{
    cout << "Task4" << endl;
    sleep(1);
}

void loadTask(vector<func_t>& map)//加载任务表
{
    map.push_back(TaskFunc1);
    map.push_back(TaskFunc2);
    map.push_back(TaskFunc3);
    map.push_back(TaskFunc4);
}


//////////////////////////////////////////////////////

//子进程执行任务

void ChildProcess(vector<func_t>& funcmap, proc_info& proc, int rid)
{
    while(true)
    {
        int mapi = 0;
        ssize_t n = read(rid, &mapi, sizeof(int));
        if(0 >= n) break;

        //printf("%d process    pid: %d   task id: %d=> ", pi, getpid(), mapi);
        cout << proc._name << " => ";
        funcmap[mapi]();
    }

    exit(0);
}



void ApplyChidProcess(vector<proc_info>& procmap, vector<func_t>& funcmap)
{
    vector<int> wfd;
    for(int i = 0;i < PROCESS_NUM;++i)
    {
        int pfd[2];
        int ret = pipe(pfd);
        assert(0 == ret);
        (void)ret;

        
        pid_t id = fork();

        procmap.push_back(proc_info(id, pfd[1], i));

        if(0 == id)
        {
            //子进程
            //cout << "fork error?" <<endl;

            //关闭跟前面管道的写（连接）
            for(int j = 0;j < wfd.size();++j) close(wfd[j]);

            //cout << "close error?" <<endl;
            //关闭管道写
            close(pfd[1]);

            //cout << "ChildProcess in error?" <<endl;
            //子进程执行任务
            ChildProcess(funcmap, procmap[i], pfd[0]);
        }

        close(pfd[0]);
        wfd.push_back(pfd[1]);
    }
}

void AllocTask(vector<proc_info>& procmap)
{
    for(int i = 0;i < 15;++i)
    {
        int P = i % PROCESS_NUM;
        int T = i % 4;
        //cout << "writing error?" << endl;
        ssize_t n = write(procmap[P]._wfd, &T, sizeof(int));
        if(n != sizeof(int)) break;
        //cout << "writed error?" << endl;

        sleep(1);
    }

    //cout << "error" <<endl;
    // while(true)
    // {
    //     int i = 0;
    //     while(true)
    //     {
    //         int P = i % 4;
    //         int T = i % PROCESS_NUM;
    //         //cout << "writing error?" << endl;
    //         ssize_t n = write(procmap[P]._wfd, &T, sizeof(int));
    //         if(n != sizeof(int)) break;
    //         //cout << "writed error?" << endl;

    //         sleep(1);

    //         ++i;
    //         //cout << "parent process:" << i << endl;
    //     }
    //     cout << "error?" << endl;
    // }
}

void RecycleResource(vector<proc_info>& procmap)
{
    
    for(int i = 0;i < procmap.size();++i)
        close(procmap[i]._wfd);

    for(int i = 0;i < procmap.size();++i)
        waitpid(procmap[i]._pid, nullptr, 0);
    
    //cout <<"wait sucess"<<endl;
}



int main()
{
    //1.初始化任务，申请子进程资源以及存储子进程、匿名管道信息
    vector<func_t> funcmap;
    loadTask(funcmap);//装载任务

    vector<proc_info> procmap;
    ApplyChidProcess(procmap, funcmap);

    //2.父进程分配任务，子进程执行任务
    AllocTask(procmap);


    //3.任务结束，关闭文件，回收子进程
    RecycleResource(procmap);

    return 0;
}