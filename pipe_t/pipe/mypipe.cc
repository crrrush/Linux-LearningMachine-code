#include<iostream>
#include<cstdio>
#include<cstring>
#include<cassert>
#include<cstdlib>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>


using std::cout;
using std::endl;

//读快 写慢  read阻塞式等待
//读慢 写快  write阻塞式等待
//写关闭 读完 read返回0
//读关闭 Linux直接发送13号信号终止进程


int main()
{
    //1.申请匿名管道文件
    int pfd[2];
    int ret = pipe(pfd);
    assert(0 == ret);

    //2.fork子进程，进行通信
    pid_t id = fork();
    if(0 == id)
    {
        //子进程
        //关闭读端
        close(pfd[0]);

        int cnt = 0;
        const char* msg = "Hello!";
        while(true)
        {
            ++cnt;
            char buffer[1024];
            sprintf(buffer, "child process<%d> sending <%d>message: %s", getpid(), cnt, msg);
            //snprintf(buffer, sizeof buffer, "child process<%d> sending <%d>message: %s", getpid(), cnt, msg);

            cout << "writing -> " << cnt << endl;
            ssize_t s = write(pfd[1], buffer, strlen(buffer));
            assert(0 <= s);
            cout << "writed success" << endl;


            sleep(3);
            if(5 == cnt) break;
            //break;
        }

        close(pfd[1]);
        printf("child process<%d> had close pipe and sleeping now\n", getpid());
        sleep(10);

        exit(0);
    }

    //父进程
    //关闭写端
    close(pfd[1]);
    
    while(true)
    {
        //break;

        char buffer[1024];
        //cout << "reading" <<endl;
        ssize_t s = read(pfd[0], buffer, sizeof(buffer) - 1);
        if(0 < s) buffer[s] = 0;
        else break;
        //cout << "read success"<<endl;

        // cout << "parent process<" << getpid() << "> sleeping" <<endl;
        sleep(2);

        cout << "parent process<" << getpid() << "> receiving message: " << buffer << endl;
    }

    //关闭读端
    close(pfd[0]);

    int status = 0;
    //阻塞式等待子进程
    pid_t rid = waitpid(id, &status, 0);
    assert(0 <= rid);
    cout << (status & 0x7F) << endl;

    return 0;
}