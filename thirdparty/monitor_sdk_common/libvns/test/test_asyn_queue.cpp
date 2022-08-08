//
// Created by 吴婷 on 2020-07-06.
//

#include "utils/asyn_queue.h"

//实现一个任务接口
class Task : public WorkItem {
public:
    Task(const std::string& name) : _name(name) {}

public:
    //这个方法里面实现具体任务
    void run()
    {
        std::cout << "异步处理任务[" << _name << "]..." << std::endl;
    }

private:
    std::string _name;
};

int main(int argc, char** argv)
{
    //使用智能指针
    std::shared_ptr<Task> A = std::make_shared<Task>("A");
    std::shared_ptr<Task> B = std::make_shared<Task>("B");
    std::shared_ptr<Task> C = std::make_shared<Task>("C");

    //启动任务队列
    TaskQueue::Instance()->start();

    //将任务放入任务队列
    TaskQueue::Instance()->push(A);
    TaskQueue::Instance()->push(B);
    TaskQueue::Instance()->push(C);

    //等待所有任务执行完毕
    while (TaskQueue::Instance()->size() > 0)
    {
        std::chrono::milliseconds dura(1);
        std::this_thread::sleep_for(dura);
    }

    return 0;
}