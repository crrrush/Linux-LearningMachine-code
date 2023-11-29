

#include <iostream>
#include <vector>
#include "mutex_s.hpp"
#include "pthread_s.hpp"


using namespace std;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;







int i = 0;

void *func1(void *args)
{
    thread_data* data = (thread_data*)args;
    while(true)
    {
        pthread_mutex_lock(&mtx);
        while(i) pthread_cond_wait(&cond, &mtx);

        cout << "i'm " << data->_name << endl;
        
        if(i != 0) i = 0;

        // pthread_cond_broadcast(&cond);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);
        sleep(1);
    }
    return nullptr;
}

int main()
{
    pthread t1(func1, nullptr, "thread-A");
    pthread t2(func1, nullptr, "thread-B");


    t1.join();
    t2.join();


    return 0;
}





// int i = 3;
// void *func1(void *args)
// {
//     thread_data* data = (thread_data*)args;
//     while(i > 0)
//     {
//         pthread_mutex_lock(&mtx);
//         cout << data->_name << " in\n";
//         while(i != data->_num && i != 0)
//         {
//             cout << data->_name << " block\n";
//             pthread_cond_wait(&cond, &mtx);
//             cout << data->_name << " unblock and see i:" << i << endl;
//         }
//         if(i != 0) cout << "<" << data->_name << ">   i: " << i-- << endl;
//         cout << data->_name << " out\n--------------------------\n";
//         pthread_cond_broadcast(&cond);
//         // pthread_cond_signal(&cond);
//         pthread_mutex_unlock(&mtx);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread t1(func1, nullptr, "thread", 1);
//     pthread t2(func1, nullptr, "thread", 2);
//     pthread t3(func1, nullptr, "thread", 3);

//     t1.join();
//     cout << "t1.join() sucess\n";
//     t2.join();
//     cout << "t2.join() sucess\n";
//     t3.join();
//     cout << "t3.join() sucess\n";

//     return 0;
// }



// int i = 12;

// void *func1(void *args)
// {
//     thread_data* data = (thread_data*)args;
//     while(i)
//     {
//         pthread_mutex_lock(&mtx);
//         cout << data->_name << " in\n";
//         while(i % 2 != data->_num % 2)
//         {
//             cout << data->_name << " block\n";
//             pthread_cond_wait(&cond, &mtx);
//             if(i == 0) return nullptr;
//         }
//         cout << "<" << data->_name << ">   i: " << i-- << endl;

//         cout << data->_name << " out\n--------------------------\n";
//         pthread_cond_broadcast(&cond);
//         // pthread_cond_signal(&cond);
//         pthread_mutex_unlock(&mtx);
//     }


//     return nullptr;
// }

// int main()
// {
//     pthread t1(func1, nullptr, "thread-A", 1);
//     pthread t2(func1, nullptr, "thread-B", 2);

//     t1.join();
//     cout << "t1.join() sucess\n";
//     t2.join();
//     cout << "t2.join() sucess\n";

//     return 0;
// }



// int t = 12;
// void *func1(void *args)
// {
//     thread_data* data = (thread_data*)args;
//     while(true)
//     {
//         lock_guard_s lock((mutex_s*)data->_arg);
//         if(t == 0) return nullptr;
//         if(t % 2 == data->_num % 2) cout << data->_name << " t: " << t-- << endl;
//     }
//     return nullptr;
// }
// int main()
// {
//     mutex_s* p = new mutex_s;
//     pthread t1(func1, (void*)p, "thread", 1);
//     pthread t2(func1, (void*)p, "thread", 2);
//     pthread t3(func1, (void*)p, "thread", 3);
//     t1.join();
//     t2.join();
//     t3.join();
//     return 0;
// }