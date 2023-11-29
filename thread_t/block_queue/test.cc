

#include <iostream>
#include <vector>
#include <memory>
#include "task.hpp"
#include "pthread_s.hpp"
#include "block_queue.hpp"

using namespace std;



void* consumer(void* args)
{
    thread_data* pd = static_cast<thread_data*>(args);
    block_queue<func_t>* bq = static_cast<block_queue<func_t>*>(pd->_arg);
    task_pool& p = task_pool::get_pool();
    while(true)
    {
        // 获取任务
        cout << "consumer 尝试获取任务\n";
        func_t& f = bq->pop();
        // cout << "consumer 获取任务成功\n";
        
        // 执行任务
        f();
        // cout << "consumer 执行任务成功\n";
        // sleep(3);
    }
    return nullptr;
}

void* producter(void* args)
{
    thread_data* pd = static_cast<thread_data*>(args);
    block_queue<func_t>* bq = static_cast<block_queue<func_t>*>(pd->_arg);
    task_pool& p = task_pool::get_pool();
    while(true)
    {
        // 生成任务
        func_t& f = p[rand() % p.size()];
        // cout << "producter 生成任务成功\n";
        
        // 推送任务
        // cout << "producter 尝试推送任务\n";
        bq->push(f);
        // cout << "producter 推送任务成功\n";

        sleep(3);
    }
    return nullptr;
}




int main()
{
    srand((unsigned int)time(nullptr));
    block_queue<func_t>* pbq = new block_queue<func_t>(); // delete? unique_ptr?

    vector<unique_ptr<pthread>> vC(3);
    vector<unique_ptr<pthread>> vP(3);

    for(int i = 0;i < vC.size();++i)
        vC[i] = unique_ptr<pthread>(new pthread(consumer, pbq, "consumer thread", i));
    
    for(int i = 0;i < vP.size();++i)
        vP[i] = unique_ptr<pthread>(new pthread(producter, pbq, "producter thread", i));

    for(int i = 0;i < vP.size();++i) vP[i]->join();

    for(int i = 0;i < vC.size();++i) vC[i]->join();

    delete pbq;

    return 0;
}


// void* test(void* args)
// {
//     thread_data* pd = static_cast<thread_data*>(args);
//     cout << pd->_name << endl;
//     return nullptr;
// }