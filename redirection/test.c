#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define FILE_NAME(NUM) "log.txt"#NUM

int main()
{
    //int fd1 = open(FILE_NAME(1), O_RDONLY);
    int fd1 = open(FILE_NAME(1), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    int fd2 = open(FILE_NAME(2), O_RDONLY);

    ////输入重定向
    //dup2(fd2, 0);
    //dup2(fd1, 1);
    //char buffer[64];
    //while(NULL != fgets(buffer, sizeof(buffer) - 1, stdin))
    //    printf("%s", buffer);

    //fflush(stdout);

    //输出重定向
    dup2(fd1, 1);
    //char buffer[1024];
    //while(NULL != fgets(buffer, sizeof(buffer) - 1, stdin))
    //    printf("%s", buffer);
    
    //printf("fd1:%d\n",fd1);
    //fprintf(stdout, "fd1:%d\n", fd1);

    const char* msg = "Hello file!\n";
    write(1, msg, strlen(msg));
    write(fd1, msg, strlen(msg));

    fflush(stdout);


    close(fd1);
    close(fd2);

    //close(0);
    //int fd1 = open(FILE_NAME(1), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    //int fd1 = open(FILE_NAME(1), O_RDONLY);
    
    //char buffer[64];
    //const char* ps = "hello world!\n";
    //write(fd1, ps, strlen(ps));
   
    //输入
    //char buffer[64];
    //char* s = gets(buffer);
    //char fbuffer[64];
    //fgets(fbuffer, sizeof(fbuffer), stdin);
    //char rbuffer[64];
    //ssize_t num = read(0 , rbuffer, sizeof(rbuffer) - 1);
    //if(num > 0) rbuffer[num] = '\0';

    //输出
    //printf("buffer: %s\n", s);
    //fprintf(stdout, "fbuffer: %s\n",fbuffer);
    //char str[64];
    //sprintf(str, "rbuffer: %s", rbuffer);
    //write(1, str, strlen(str));

    //printf("this is printf\n");
    //fprintf(stdout, "this is fprintf\n");
    //const char* ps = "this is write\n";
    //write(1, ps, strlen(ps));

    //close(fd1);
    
    return 0;
}
