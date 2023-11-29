#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

extern int kill(pid_t pid, int sig);

#define NUM 1024   //命令行字符串容量
#define agNum 64   //指令以及指令参数容量

char Command[NUM];     //命令行字符串
char* myargv[agNum];   //指令以及指令参数


//#define NONE_REDIR 0
//#define INPUT_REDIR 1 //<
//#define OUTPUT_REDIR 2 //>
//#define APPEND_REDIR 3 //>>
enum REDIR
{
    NONE_REDIR,
    INIPUT_REDIR,
    OUTPUT_REDIR,
    APPEND_REDIR
};

char* filename = NULL;
enum REDIR rd_option = NONE_REDIR;


#define PCNum 64           // 管道中指令条容量
char* Pipe_Command[PCNum]; // 存储管道中各个指令以及参数

int pipe_n = 0;  //管道数量




//#define TrimSpace(POINTER ,POS) do{ while(isspace( ((POINTER)[(POS)] ))) ++(POS); }while(0)
#define TrimSpace(POINTER ,POS)\
do{\
    while( isspace( ((POINTER)[(POS)]) ) )\
            ++(POS);\
}while(0)




void Pipe_Check(char* Coms)
{
    assert(Coms);
    int begin = 0,end = strlen(Coms) - 1;
    while(begin <= end)
    {
        if(Coms[begin] == '|')
        {
            //截断字符串
            Coms[begin] = '\0';
            
            //过滤空格
            ++begin;
            TrimSpace(Coms ,begin);
            
            //存储指令
            Pipe_Command[pipe_n++] = Coms + begin;

            //printf("Pipe_Check error?\n");
        }
        else 
            ++begin;
    }
}


void Redir_Check(char* Coms)
{
    assert(Coms);
    int begin = 0,end = strlen(Coms) - 1;
    while(begin <= end)
    {
        if(Coms[begin] == '<')
        {
            //截断字符串
            Coms[begin] = '\0';
            
            //过滤空格
            ++begin;
            TrimSpace(Coms ,begin);
            
            //设置指针指向文件名字符串
            filename = Coms + begin;
            //设置重定向选项
            rd_option = INIPUT_REDIR;
            break;
        }
        else if(Coms[begin] == '>')
        {
            //截断字符串
            Coms[begin] = '\0';

            //判断>/>> 并设置选项
            ++begin;
            if(Coms[begin] == '>')
            {
                rd_option = APPEND_REDIR;
                ++begin;
            }
            else 
            {
                rd_option = OUTPUT_REDIR;
            }

            //过滤空格
            TrimSpace(Coms ,begin);
            
            //设置指针指向文件名字符串
            filename = Coms + begin;
            break;
        }
        else 
            ++begin;
    }
}

bool Command_Analysis(char* Coms) //解析子进程程序替换的程序以及参数    解析失败或不需要子进程执行的命令返回false
{
    assert(Coms);

    //重定向检查
    Redir_Check(Coms);

    int i = 1;
    myargv[0] = strtok(Coms," ");


    //ll ls 命令处理
    if(myargv[0] != NULL && strcmp(myargv[0], "ll") == 0)//?
    {
        myargv[0] = (char*)"ls";
        myargv[i++] = (char*) "-l";
    }
    if(myargv[0] != NULL && strcmp(myargv[0], "ls") == 0)
        myargv[i++] = (char*)"--color=auto";

    while( (myargv[i++] = strtok(NULL," ")) ); //命令行字符串切割

    //cd 命令处理
    if(strcmp(myargv[0], "cd") == 0)
    {
        if(myargv[1]) chdir(myargv[1]);
        return false;
    }

    return true;
}


void Child_Process()
{
    //处理重定向
    switch(rd_option)
    {
        case NONE_REDIR:
            break;
        case INIPUT_REDIR:
            {
                int fd = open(filename,O_RDONLY);
                if(fd < 0
                {
                    perror("open failed\n");
                    exit(errno);
                }
                dup2(fd, 0);
            }
            break;
        case OUTPUT_REDIR:
        case APPEND_REDIR:
            {
                int flags = O_WRONLY | O_CREAT;
                if(rd_option == OUTPUT_REDIR) flags |= O_TRUNC;
                else flags |= O_APPEND;
                int fd = open(filename, flags, 0666);
                if(fd < 0)
                {
                    perror("open failed\n");
                    exit(errno);
                }
                dup2(fd ,1);
            }
            break;
        default:
            printf("REDIR ERROR?\n");
            break;
    }
    execvp(myargv[0],myargv);
    exit(1);
}


 //             if(-1 == i)//第一个指令
//             {
//                 //关闭读
//                 close(pfd[0]);
                
//                 //管道文件写重定向到标准输出
//                 dup2(pfd[1], 1);
//             }
//             else if(pipe_n - 1 == i)//最后一个指令
//             {
//                 //关闭写
//                 close(pfd[1]);

//                 //管道文件读重定向到标准输入
//                 dup2(pfd[0], 0);
//             }
//             else
//             {
//                 //管道文件写重定向到标准输出
//                 dup2(pfd[1], 1);
//                 //管道文件读重定向到标准输入
//                 dup2(pfd[0], 0);
//             }

void Pipe_Process(int p_n,int* pfd)
{
    //初始化 文件名以及重定向选项
    filename = NULL;
    rd_option = NONE_REDIR;
    if(p_n == 0)
    {
        Command_Analysis(Command);

        Child_Process();
        // pid_t id = fork();
        // if(0 == id)
        //     Child_Process();

        // waitpid(id,NULL,0);
    }
    else
    {
        Command_Analysis(Pipe_Command[p_n -1]);

        int fds[2];
        int ret = pipe(fds);
        assert(0 == ret);

        pid_t id = fork();
        if(0 == id)
        {
            close(fds[0]);
            dup2(fds[1], 1);
            
            printf("pipe proc error?\n");
            Pipe_Process(p_n - 1, fds);
            printf("pipe proc error?\n");

            Child_Process();
        }

        if(pfd == NULL)
        {
            close(fds[1]);
            dup2(fds[0], 0);
        }
        else
        {
            dup2(fds[0], 0);
        }

        printf("waiting error?\n");
        waitpid(id,NULL,0);
        printf("waited error?\n");
    }

}



int main()
{
    while(1)
    {   
        //初始化管道数
        pipe_n = 0;
        //初始化 文件名以及重定向选项
        filename = NULL;
        rd_option = NONE_REDIR;
        
        //输出提示符
        //char* s = getenv("PWD");
        //char* d = NULL;
        //d = strtok(s, "/");
        //while(s = strtok(NULL,"/"))
        //    d = s;
        printf("[%s@%s shell]$ ",getenv("USER"), getenv("HOSTNAME"));
        fflush(stdout);
    
        //获取用户输入
        //char* str = fgets(Command, sizeof(Command) - 1, stdin);
        //assert(str);
        fgets(Command, sizeof(Command) - 1, stdin);
        //测试输入
        Command[strlen(Command) - 1] = '\0';
        //printf("%s\n",Command);
        
        Pipe_Check(Command);
        
        if(0 == pipe_n)
        {
            //解析用户命令
            if(!Command_Analysis(Command)) continue;
            
            pid_t id = fork();

            if(0 == id)
                Child_Process();

            waitpid(id,NULL,0);
        }
        else
        {
            pid_t id = fork();
            if(0 == id)
            {
                // int pfd[2];
                // int ret = pipe(pfd);
                // assert(0 == ret);

                Pipe_Process(pipe_n, NULL);

                Child_Process();
            }

            waitpid(id,NULL,0);

            // int pfd[2];
            // int ret = pipe(pfd);
            // assert(0 == ret);

            // int pfd0[2];
            // int ret = pipe(pfd0);
            // assert(0 == ret);

            // int pfd1[2];
            // ret = pipe(pfd1);
            // assert(0 == ret);

            //printf("pipe error?\n");
        //     for(int i = -1;i < pipe_n;++i)
        //     {
        //         //初始化 文件名以及重定向选项
        //         filename = NULL;
        //         rd_option = NONE_REDIR;

        //         //bool flag = true;
        //         // if(i == -1)
        //         //     flag = Command_Analysis(Command);
        //         // else
        //         //     flag = Command_Analysis(Pipe_Command[i]);
        //         //if(!flag) break;//不需要执行，退出

        //         //printf("Command_Analysis error?\n");
        //         if(-1 == i)
        //             Command_Analysis(Command);
        //         else
        //             Command_Analysis(Pipe_Command[i]);

                
        //         //printf("Command_Analysis error?\n");

        //         pid_t id = fork();
        //         if(0 == id)
        //         {
        //             // if(i % 2 == 1)
        //             // {
        //             //     if(-1 == i)//第一个指令
        //             //     {
        //             //         //关闭0管道
        //             //         close(pfd0[0]);
        //             //         close(pfd0[1]);
                            
        //             //         //关闭1管道读
        //             //         close(pfd1[0]);

        //             //         //管道文件写重定向到标准输出
        //             //         dup2(pfd1[1], 1);
        //             //     }
        //             //     else
        //             //     {
        //             //         //关闭0管道写
        //             //         close(pfd0[1]);

        //             //         //关闭1管道读
        //             //         close(pfd1[0]);

        //             //         dup2(pfd0[0], 0);
        //             //         dup2(pfd1[1], 1);
        //             //     }
        //             // }
        //             // else if(i % 2 == 0)
        //             // {
        //             //     if(pipe_n - 1 == i)//最后一个指令
        //             //     {
        //             //         //关闭0管道
        //             //         close(pfd0[0]);
        //             //         close(pfd0[1]);

        //             //         //关闭1管道写
        //             //         close(pfd1[1]);

        //             //         //管道文件读重定向到标准输入
        //             //         dup2(pfd1[0], 0);
        //             //     }
        //             //     else
        //             //     {
        //             //         //关闭0管道读
        //             //         close(pfd0[0]);

        //             //         //关闭1管道写
        //             //         close(pfd1[1]);

        //             //         dup2(pfd1[0], 0);
        //             //         dup2(pfd0[1], 1);
        //             //     }
        //             // }

        //             if(-1 == i)//第一个指令
        //             {
        //                 //关闭读
        //                 close(pfd[0]);
                        
        //                 //管道文件写重定向到标准输出
        //                 dup2(pfd[1], 1);
        //             }
        //             else if(pipe_n - 1 == i)//最后一个指令
        //             {
        //                 //关闭写
        //                 close(pfd[1]);

        //                 //管道文件读重定向到标准输入
        //                 dup2(pfd[0], 0);
        //             }
        //             else
        //             {
        //                 //管道文件写重定向到标准输出
        //                 dup2(pfd[1], 1);
        //                 //管道文件读重定向到标准输入
        //                 dup2(pfd[0], 0);
        //             }

        //             //printf("dup2 error?\n");
                    
        //             Child_Process();//执行子进程程序替换
        //         }

        //         printf("waiting error?\n");
                
        //         waitpid(id,NULL,0);
        //         kill(id ,13);
        //         printf("waited error?\n");
        //     }

        //     //waitpid(-1,NULL,0);
            
        //     // close(pfd0[0]);
        //     // close(pfd0[1]);
        //     // close(pfd1[0]);
        //     // close(pfd1[1]);

        //     close(pfd[0]);
        //     close(pfd[1]);
        }

        // int i = 1;
        // myargv[0] = strtok(Command," ");
        // //ll ls 命令处理
        // if(myargv[0] != NULL && strcmp(myargv[0], "ll") == 0)//?
        // {
        //     myargv[0] = (char*)"ls";
        //     myargv[i++] = (char*) "-l";
        // }
        // if(myargv[0] != NULL && strcmp(myargv[0], "ls") == 0)
        //     myargv[i++] = (char*)"--color=auto";

        // while( (myargv[i++] = strtok(NULL," ")) ); //命令行字符串切割
        // //cd 命令处理
        // if(strcmp(myargv[0], "cd") == 0)
        // {
        //     if(myargv[1]) chdir(myargv[1]);
        //     continue;
        // }
        

        //测试是否解析成功
        //for(i = 0;myargv[i];++i)
        //    printf("myargv[%d]:%s\n",i, myargv[i]);
        
        //fork出子进程执行命令
        // pid_t id = fork();

        // if(0 == id)
        //     Child_Process();

        // waitpid(id,NULL,0);
        
    }



    
    return 0;
}

