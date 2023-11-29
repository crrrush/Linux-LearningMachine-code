#include  <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define FILE_NAME(NUM) "log.txt"#NUM

int main()
{
    //O_WRONLY 和 O_RDONLY 同时用？
    int fd1 = open(FILE_NAME(1), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    int fd2 = open(FILE_NAME(2), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    int fd3 = open(FILE_NAME(3), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    int fd4 = open(FILE_NAME(4), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    int fd5 = open(FILE_NAME(5), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    int fd6 = open(FILE_NAME(6), O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC, 0666);
    //int fd = open(FILE_NAME, O_RDONLY);
    
    printf("stdin->fd:%d\n", stdin->_fileno);
    printf("stdout->fd:%d\n", stdout->_fileno);
    printf("stderr->fd:%d\n", stderr->_fileno);

    printf("%s->fd:%d\n",FILE_NAME(1), fd1);
    printf("%s->fd:%d\n",FILE_NAME(2), fd2);
    printf("%s->fd:%d\n",FILE_NAME(3), fd3);
    printf("%s->fd:%d\n",FILE_NAME(4), fd3);
    printf("%s->fd:%d\n",FILE_NAME(5), fd4);
    printf("%s->fd:%d\n",FILE_NAME(6), fd5);


    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    close(fd5);
    close(fd6);

    ////读取文件
    //char buffer[64];
    //ssize_t num = read(fd, buffer, sizeof(buffer) - 1);
    //if(num > 0) buffer[num] = '\0';
    //printf("%s", buffer);


    //向文件写入内容
    //int cnt = 5;
    //char buffer[64];
    //while(cnt)
    //{
    //   sprintf(buffer, "%s:%d\n", FILE_NAME, cnt--);
    //    write(fd, buffer, strlen(buffer));
    //}


    //close(fd);


    return 0;
}
