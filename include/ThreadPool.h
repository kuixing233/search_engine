#ifndef _THREADPOOL_H__
#define _THREADPOOL_H__

#include "Thread.h"
#include "TaskQueue.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class ThreadPool
{
public:
    using Task = function<void()>;
public:
    ThreadPool(size_t threadNum, size_t queSize);
    ~ThreadPool();
    void start(); // 初始化并启动所有工作线程
    void stop(); // 停止线程池
    void addTask(Task &&); // 添加任务

private:
    Task getTask(); // 获取任务
    void threadFunc(); // 具体任务

private:
    size_t _threadNum; // 工作线程数目
    size_t _queSize; // 任务队列大小
    vector<unique_ptr<Thread>> _threads; // 工作线程容器
    TaskQueue _taskQue; // 任务队列
    bool _isExit; // 线程池退出标志
};

#endif
