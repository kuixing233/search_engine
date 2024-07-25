#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
: _queSize(queSize)
, _que()
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
, _flag(true)
{

}

TaskQueue::~TaskQueue()
{

}

bool TaskQueue::empty() const
{
    return 0 == _que.size();
}

bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

void TaskQueue::push(ElemType &&value)
{
    // 使用RAII思想实现自动上锁和解锁
    MutexLockGuard mguard(_mutex);
    /* _mutex.lock(); */ 

    // 虚假唤醒
    while (full())
    {
        _notFull.wait();
    }

    _que.push(value);

    // 唤醒消费者
    _notEmpty.notify();

    /* _mutex.unlock(); */
}

TaskQueue::ElemType TaskQueue::pop()
{
    MutexLockGuard mguard(_mutex);

    // _flag表示线程池是否还在运行
    while (_flag && empty())
    {
        _notEmpty.wait();
    }

    if (_flag)
    {
        ElemType tmp = _que.front();
        _que.pop();

        // 唤醒生产者
        _notFull.notify();

        return tmp;
    }
    else
    {
        return nullptr;
    }
}

void TaskQueue::wakeup()
{
    // 当执行到这里，代表线程池要退出了
    _flag = false; 
    _notEmpty.notifyAll();
}
