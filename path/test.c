#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

struct str{
    int len;
    char s[0];
};

struct foo {
    struct str *a;
};

int main(int argc, char** argv) {
    struct foo f={0};
    if (f.a->s) {
        //printf( f.a->s);
        printf("%x\n", f.a->s);
    }
    return 0;
}


//int main(int argc, char* argv[],char* env[])
//int main()
//{

    //extern char** environ;
    //for(int i = 0;environ[i];++i)
    //{
    //    printf("environ[%d]: %s\n",i,environ[i]);
    //}
    //for(int i = 0;env[i];++i)
    //{
    //    printf("env[%d]: %s\n",i,env[i]);
    //}
    

    //for(int i = 0;i < argc;++i)
    //{
    //    printf("argv[%d]: %s\n",i,argv[i]);
    //}


    //char *who = getenv("HOME");

    //printf("%s\n",who);

    //return 0;
//}

