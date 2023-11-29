#include "add.h"
#include "sub.h"

#include <stdio.h>

// libmymath.so

int main()
{
    printf("result: %d\n", ADD(1, 1));
    printf("result: %d\n", SUB(1, 1));


    return 0;
}

