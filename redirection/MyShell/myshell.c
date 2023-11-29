#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define NUM 1024
#define agNum 64

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

//#define TrimSpace(POINTER ,POS) do{ while(isspace( ((POINTER)[(POS)] ))) ++(POS); }while(0)

#define TrimSpace(POINTER ,POS)\
do{\
    while( isspace( ((POINTER)[(POS)]) ) )\
            ++(POS);\
}while(0)

char Command[NUM];
char* myargv[agNum];

char* filename = NULL;
enum REDIR rd_option = NONE_REDIR;

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


int main()
{
    while(1)
    {   
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
         
        Redir_Check(Command);

        //解析用户命令
        int i = 1;
        myargv[0] = strtok(Command," ");



        if(myargv[0] != NULL && strcmp(myargv[0], "ll") == 0)//?
        {
            myargv[0] = (char*)"ls";
            myargv[i++] = (char*) "-l";
        }
        if(myargv[0] != NULL && strcmp(myargv[0], "ls") == 0)
            myargv[i++] = (char*)"--color=auto";

        while( (myargv[i++] = strtok(NULL," ")) ); //命令行字符串切割

        if(strcmp(myargv[0], "cd") == 0)
        {
            if(myargv[1]) chdir(myargv[1]);
            continue;
        }


        //测试是否解析成功
        //for(i = 0;myargv[i];++i)
        //    printf("myargv[%d]:%s\n",i, myargv[i]);
        
        pid_t id = fork();

        if(0 == id)
        {
            //处理重定向
            switch(rd_option)
            {
                case NONE_REDIR:
                    break;
                case INIPUT_REDIR:
                    {
                        int fd = open(filename,O_RDONLY);
                        if(fd < 0)
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

        waitpid(id,NULL,0);
        
    }



    
    return 0;
}

