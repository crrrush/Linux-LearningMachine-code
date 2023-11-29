#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define FILE_NAME(NUM) "log.txt"#NUM

int main()
{
    int fd1 = open(FILE_NAME(1), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    
    dup2(fd1, 1);
    
    printf("output from printf\n");
    fprintf(stdout, "output from fprintf\n");
    fputs("output from fputs\n", stdout);

    const char* msg = "output from write\n";
    write(1, msg, strlen(msg));

    //fflush(stdout);
    
    fork();

    close(fd1);
    
    return 0;
}
