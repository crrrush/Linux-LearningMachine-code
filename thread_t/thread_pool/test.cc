

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "task.hpp"
#include "thread_pool.hpp"

using namespace std;

typedef thread_pool<func_t> TP;

int main()
{
    srand((unsigned int)time(nullptr));
    TP::GetInstance()->run();
    task_pool &t = task_pool::get_pool();

    while (true)
    {
        // printf("-------------------main while in------------------------\n");

        TP::GetInstance()->push(t[rand() % t.size()]);
        
        printf("-----------------function push sucess----------------------\n");


        sleep(1);
    }

    return 0;
}
