#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//int register quit = 0;
//int quit = 0;
volatile int quit = 0;

void handler(int signo)
{
    printf("pid：%d 进程捕捉到%d号信号，正在处理中...\n", getpid(), signo);
    printf("quit: %d -> ", quit);
    quit = 1;
    printf("%d\n", quit);
}

int main()
{
    //register int quit = 0;
    signal(2, handler);

    while(!quit);

    printf("注意！我是正常退出的！\n");
    return 0;
}
