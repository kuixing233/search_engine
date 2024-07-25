#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(queSize)
, _isExit(false)
{
    _threads.reserve(threadNum);
}

ThreadPool::~ThreadPool()
{
    // 防止没有手动调用stop方法
    if (!_isExit) 
    {
        stop(); // 直接调用stop即可
    }
}

void ThreadPool::start()
{
    for (size_t idx = 0; idx != _threadNum; ++ idx)
    {
        unique_ptr<Thread> up(new Thread(std::bind(&ThreadPool::threadFunc, this)));
        _threads.push_back(std::move(up)); // 注意容器只能存unique_ptr的右值 
    }

    for (auto &th: _threads)
    {
        th->start();
    }
}

void ThreadPool::stop()
{
    // 主线程执行到了stop()，但此时任务还没完成，不能退出
    // 让主线程睡眠
    while (!_taskQue.empty())
    {
        sleep(1);
    }

    _isExit = true;
    _taskQue.wakeup(); // 唤醒所有睡在_notEmpty上的进程

    for (auto &th: _threads)
    {
        th->join();
    }
}

void ThreadPool::addTask(Task && cb)
{
    if (cb)
        _taskQue.push(std::move(cb));
}

ThreadPool::Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

void ThreadPool::threadFunc()
{
    // 只要线程池没退出，工作线程就一直从任务队列拿任务
    // 拿不到就会等待
    

    // 在主线程睡觉的1s内，工作线程将所有任务完成
    // 但此时_isExit = false，工作线程继续从任务队列
    // 拿任务，就会睡在_notEmpty()条件变量上
    while (!_isExit)
    /* while (!_taskQue.empty()) */
    {
        Task taskcb = getTask();
        if (taskcb)
        {
            taskcb();
        }
    }
}
