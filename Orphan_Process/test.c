#include <stdio.h>
#include <unistd.h>

#include <assert.h>

int main()
{
    pid_t id = fork();
    
    assert(0 <= id);

    if(0 == id)
    {
        while(1)
        {
            printf("i'm child process. pid: %d ppid: %d\n",getpid(), getppid());
            sleep(2);
        }
    }
    else 
    {
        while(1)
        {
            printf("i'm parent process. pid: %d ppid: %d\n",getpid(), getppid());
            sleep(3);
        }
    }
    return 0;
}
