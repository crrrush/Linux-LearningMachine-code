#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>

#define NUM 1024
#define agNum 64

char Command[NUM];

char* myargv[agNum];


int main()
{
    while(1)
    {    
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

        while(myargv[i++] = strtok(NULL," ")); //命令行字符串切割

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
            execvp(myargv[0],myargv);
            exit(1);
        }

        waitpid(id,NULL,0);
        
    }



    
    return 0;
}

