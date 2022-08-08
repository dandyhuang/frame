//
// Created by 吴婷 on 2020-07-06.
//
#include <ctime>
#include <queue>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <iostream>

//using namespace std;

//任务接口
class WorkItem
{
public:
    virtual ~WorkItem(){};

    //接口方法必须在子类实现
    virtual void run() = 0;

public:
    //任务清理接口
    virtual void clean()
    {
    }
    //判断任务是否可执行(返回真时任务才会执行)
    virtual bool runnable()
    {
        return true;
    }
};

//自旋锁类
class SpinMutex
{
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
        while (flag.test_and_set(std::memory_order_acquire));
    }
    void unlock()
    {
        flag.clear(std::memory_order_release);
    }
};

//任务队列
class TaskQueue
{
private:
    size_t maxsz;
    size_t threads;
    mutable SpinMutex mtx;
    std::queue<std::shared_ptr<WorkItem>> que;

    TaskQueue()
    {
        this->maxsz = 0;
    }
    bool pop(std::shared_ptr<WorkItem>& item)
    {
        std::lock_guard<SpinMutex> lk(mtx);
        if (que.empty()) return false;
        item = que.front(); que.pop();
        return true;
    }

public:
    //实现单例模式
    static TaskQueue* Instance()
    {
        static TaskQueue obj;
        return &obj;
    }

public:
    //中止任务处理
    void stop()
    {
        threads = 0;
    }
    //清空队列
    void clear()
    {
        std::lock_guard<SpinMutex> lk(mtx);
        while (que.size() > 0) que.pop();
    }
    //判断队列是否为空
    bool empty() const
    {
        std::lock_guard<SpinMutex> lk(mtx);
        return que.empty();
    }
    //获取队列深度
    size_t size() const
    {
        std::lock_guard<SpinMutex> lk(mtx);
        return que.size();
    }
    //获取任务线程线
    size_t getThreads() const
    {
        return threads;
    }
    //任务对象入队
    bool push(std::shared_ptr<WorkItem> item)
    {
        std::lock_guard<SpinMutex> lk(mtx);
        if (maxsz > 0 && que.size() >= maxsz) return false;
        que.push(item);
        return true;
    }
    //启动任务队列(启动处理线程)
    void start(size_t threads = 4, size_t maxsz = 10000)
    {
        this->threads = threads;
        this->maxsz = maxsz;

        for (size_t i = 0; i < threads; i++)
        {
            std::thread(std::bind(&TaskQueue::run, this)).detach();
        }
    }

public:
    //这个方法里面处理具体任务
    void run()
    {
        std::shared_ptr<WorkItem> item;

        while (threads > 0)
        {
            if (pop(item))
            {
                if (item->runnable())
                {
                    item->run();
                    item->clean();
                }
                else
                {
                    std::lock_guard<SpinMutex> lk(mtx);
                    que.push(item);
                }
            }
            else
            {
                std::chrono::milliseconds dura(1);
                std::this_thread::sleep_for(dura);
            }
        }
    }
};