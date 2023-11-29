#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    printf("process runing\n");
    pid_t id = fork();
    if(0 == id)
    {
        sleep(1);
        execl("/usr/bin/ls","ls","-a","-l","--color=auto",NULL);
        exit(1);
    }

    int status = 0;
    pid_t ret = waitpid(id, &status, 0);
    if(0 < id)
    {
        printf("wait successs: %d,sig number: %d,child exit code: %d\n",ret, (status & 0x7F), (status>>8 & 0xFF));
    }


//    if(id == 0)
//   {
//        printf("I'm child process. pid: %d  ppid: %d\n",getpid(), getppid());  
//    }
//    else if(id > 1)
//    {
//        printf("I'm parent process. pid: %d  ppid: %d\n",getpid(), getppid());
//    }


    return 0;
}

