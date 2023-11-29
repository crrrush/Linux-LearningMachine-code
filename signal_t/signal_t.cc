#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>


using namespace std;


////////////////////////////////////////////信号捕捉

// void secondCount(int n)
// {
//     while(n >= 0)
//     {
//         printf("处理中，倒计时：%2d\r", n--);
//         fflush(stdout);
//         sleep(1);
//     }
// }

// void handler(int signo)
// {
//     printf("进程（pid：%d）捕捉到信号，信号编号：%d\n", getpid(), signo);
//     secondCount(12);
// }

// void sigactionInit(struct sigaction& act)
// {
//     act.sa_handler = nullptr;
//     act.sa_sigaction = nullptr;
//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;
//     act.sa_restorer = nullptr;
// }

// int main()
// {
//     struct sigaction act, oact;
//     //初始化
//     sigactionInit(act);
//     sigactionInit(oact);

//     act.sa_handler = handler;
//     sigaddset(&act.sa_mask, 3);
//     sigaction(SIGINT, &act, &oact);

//     while(true)
//     {
//         printf("process runing... pid: %d\n", getpid());
//         sleep(1);
//     } 

//     return 0;
// }



////////////////////////////

#define MAXSIGN 31
#define MINSIGN 1


void blocksignal(sigset_t& block, sigset_t& oblock, const vector<int>& vsige)
{
    // 1.1 初始化
    sigemptyset(&block);
    sigemptyset(&oblock);
    // 1.2 设置需要屏蔽的信号
    for(const auto& sig : vsige) sigaddset(&block, sig);
    // 1.3 屏蔽信号设置
    sigprocmask(SIG_SETMASK, &block, &oblock);
}

void cancelblock(sigset_t& block, sigset_t& oblock)//取消阻塞
{
    sigprocmask(SIG_SETMASK, &oblock, &block);
}


void showpending(sigset_t& pending)
{
    cout << "mypid: " << getpid() << " -> ";
    for(int i = MAXSIGN;i >= MINSIGN;--i)
    {
        if(sigismember(&pending, i)) cout << '1';
        else cout << '0';
    }
    cout << "\n";
}

void sighandler(int signo)
{
    cout << "捕捉到信号：" << signo << endl;
}

int main()
{
    vector<int> vsige = {2, 3};
    for(const auto& sig : vsige) signal(sig, sighandler);
    // 1. 对信号进行屏蔽
    sigset_t block, oblock, pending;
    blocksignal(block, oblock, vsige);

    int cnt = 0;

    while(true)
    {
        //初始化pending
        sigemptyset(&pending);
        //获得pending位图
        sigpending(&pending);
        //打印位图
        showpending(pending);

        cout << "count: " << ++cnt << endl;
        if(cnt >= 12)
        {
            cancelblock(block, oblock);
            cout << "解除阻塞成功，尝试再次进行阻塞" << endl;
            blocksignal(block, oblock, vsige);
            cnt = 0;
        }

        sleep(1);
    }

    return 0;
}





/////////////////////////////////////////信号Action

// // Core
// // ulimit -a 查看 core file size 
// // ulimit -c 设置 core file size 大小
// // 运行会产生进行Core处理（核心转储）动作的异常的程序
// // 生成core.pid 文件，是二进制文件，vim打开是乱码, od可以查看是二进制机器指令，那怎么用？
// // 使用debug模式编译的程序，可以使用gdb调试该程序，载入这个文件可以直接定位到出现问题的地方
// int main()
// {
//     int n = 10;
//     n /= 0;

//     return 0;
// }



/////////////////////////////////////////信号产生

// int cnt = 0;

// void handler(int signo)
// {
//     cout << "捕捉到信号，信号编号为" << signo << "，进程pid: " << getpid() << " count: "<< cnt << endl;
//     //cout << "捕捉到信号，信号编号为" << signo << "，进程pid: " << getpid() << endl;
//     //sleep(1);
//     exit(0);
// }



// // 4.软件条件  SIGALRM 14号信号 闹钟 用软件实现
// int main()
// {
//     signal(SIGALRM, handler);

//     alarm(1);

    
//     while(true) cnt++; // 对比下来可以看出IO很慢
//     // while(true)
//     // {
//     //     cout << "i'm processing! count: " << cnt++ << endl;
//     // }

//     return 0;
// }


// // 3. 硬件异常产生的信号
// // 为什么硬件异常会导致不断发送信号（可以看到在不断地调用handler）
// // 没有解决问题 进程切换时，进程上下文不断载入CPU中的寄存器 从而一直引发异常
// int main()
// {
//     // signal(SIGFPE, handler);//8号信号，浮点错误
//     // cout << "i'm processing!" << endl;
//     // sleep(1);
//     // int n = 10;
//     // n /= 0;
//     // 除0 OS又是如何知道程序进行除零操作了呢？ 对应的什么硬件出现异常？


//     // signal(SIGSEGV, handler);//11号信号，段错误
//     // cout << "i'm processing!" << endl;
//     // sleep(1);
//     // int* p = nullptr;
//     // *p = 100; //空指针的解引用
//     // OS如何知道程序对空指针解引用的？如何报的异常？ 对应的什么硬件出现异常？

//     return 0;
// }


// // 2.系统调用向指定进程发送信号
// void direction()
// {
//     cout << "usage help: mykill pid signo" << endl;
//     exit(0);
// }

// int main(int argc, char* argv[])//mykill
// {
//     if(argc != 3)
//         direction();

//     pid_t pid = atoi(argv[1]);
//     int signo = atoi(argv[2]);
//     int n = kill(pid, signo);
//     assert(0 == n);
    
//     return 0;
// }



//// 1.键盘热键翻译成信号向前台进程发送
// int main()
// {
//     // signal函数注册收到该信号的处理函数
//     // signal(2, handler);
//     // int cnt = 1;
//     // while(true)
//     // {
//     //     cout << "死循环中，循环次数：" << cnt++ << endl;
//     //     sleep(1);
//     // }
//     return 0;
// }

// void sigcb(int signo)
// {
//     printf("进程（pid：%d）捕捉到%d号信号\n", getpid(), signo);
// }

// int main()
// {
//     // signal函数注册收到该信号的处理函数
//     signal(2, sigcb);
//     int cnt = 1;
//     while(true)
//     {
//         cout << "死循环中，循环次数：" << cnt++ << endl;
//         sleep(1);
//     }
//     return 0;
// }
