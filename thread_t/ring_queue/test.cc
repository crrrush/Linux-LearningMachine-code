

#include <iostream>
#include <vector>
#include <memory>
#include "task.hpp"
#include "pthread_s.hpp"
#include "ring_queue.hpp"
#include "cal.hpp"

using namespace std;

void *consumer(void *args)
{
    thread_data *pd = static_cast<thread_data *>(args);
    ring_queue<equation> *rq = static_cast<ring_queue<equation> *>(pd->_arg);
    task_pool &p = task_pool::get_pool();
    while (true)
    {
        // 获取任务
        // cout << "consumer 尝试获取任务\n";
        equation &e = rq->pop();
        // cout << "consumer 获取任务成功\n";

        // 执行任务
        cout << e._x << e._op << e._y << " = ?\n";
        cout << "answer: " << calculator(e) << endl;
        // cout << "consumer 执行任务成功\n";
        sleep(1);
    }
    return nullptr;
}

void *producter(void *args)
{
    thread_data *pd = static_cast<thread_data *>(args);
    ring_queue<equation> *rq = static_cast<ring_queue<equation> *>(pd->_arg);
    task_pool &p = task_pool::get_pool();
    while (true)
    {
        // 生成任务
        unique_ptr<equation> pe(new equation(calculator)); // unique_ptr???wrong
        // cout << "producter 生成任务成功\n";

        // 推送任务
        // cout << "producter 尝试推送任务\n";
        rq->push(*pe);
        // cout << "producter 推送任务成功\n";

        // sleep(5);
        sleep(1);
    }
    return nullptr;
}

int main()
{
    srand((unsigned int)time(nullptr));
    ring_queue<equation> *prq = new ring_queue<equation>(12); // delete? unique_ptr?

    vector<unique_ptr<pthread>> vC(6);
    vector<unique_ptr<pthread>> vP(2);

    for (int i = 0; i < vC.size(); ++i)
        vC[i] = unique_ptr<pthread>(new pthread(consumer, prq, "consumer thread", i));

    for (int i = 0; i < vP.size(); ++i)
        vP[i] = unique_ptr<pthread>(new pthread(producter, prq, "producter thread", i));

    for (int i = 0; i < vP.size(); ++i)
        vP[i]->join();

    for (int i = 0; i < vC.size(); ++i)
        vC[i]->join();

    delete prq;

    return 0;
}

// void* consumer(void* args)
// {
//     thread_data* pd = static_cast<thread_data*>(args);
//     ring_queue<func_t>* rq = static_cast<ring_queue<func_t>*>(pd->_arg);
//     task_pool& p = task_pool::get_pool();
//     while(true)
//     {
//         // 获取任务
//         // cout << "consumer 尝试获取任务\n";
//         func_t& f = rq->pop();
//         // cout << "consumer 获取任务成功\n";

//         // 执行任务
//         f();
//         // cout << "consumer 执行任务成功\n";
//         // sleep(1);
//     }
//     return nullptr;
// }

// void* producter(void* args)
// {
//     thread_data* pd = static_cast<thread_data*>(args);
//     ring_queue<func_t>* rq = static_cast<ring_queue<func_t>*>(pd->_arg);
//     task_pool& p = task_pool::get_pool();
//     while(true)
//     {
//         // 生成任务
//         func_t& f = p[rand() % p.size()];
//         // cout << "producter 生成任务成功\n";

//         // 推送任务
//         // cout << "producter 尝试推送任务\n";
//         rq->push(f);
//         // cout << "producter 推送任务成功\n";

//         // sleep(5);
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     srand((unsigned int)time(nullptr));
//     ring_queue<func_t>* prq = new ring_queue<func_t>(12); // delete? unique_ptr?

//     vector<unique_ptr<pthread>> vC(6);
//     vector<unique_ptr<pthread>> vP(2);

//     for(int i = 0;i < vC.size();++i)
//         vC[i] = unique_ptr<pthread>(new pthread(consumer, prq, "consumer thread", i));

//     for(int i = 0;i < vP.size();++i)
//         vP[i] = unique_ptr<pthread>(new pthread(producter, prq, "producter thread", i));

//     for(int i = 0;i < vP.size();++i) vP[i]->join();

//     for(int i = 0;i < vC.size();++i) vC[i]->join();

//     delete prq;

//     return 0;
// }
