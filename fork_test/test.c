#include <stdio.h>
#include <unistd.h>

#include <assert.h>

int main()
{
    pid_t id = fork();

    assert(id >= 0);

    if(id == 0)
    {
        printf("I'm child process. pid: %d  ppid: %d\n",getpid(), getppid());  
    }
    else if(id > 1)
    {
        printf("I'm parent process. pid: %d  ppid: %d\n",getpid(), getppid());
    }


    return 0;
}

