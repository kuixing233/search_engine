#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include "MutexLockGuard.h"

#include <queue>
#include <functional>

using std::queue;
using std::function;

class TaskQueue
{
public:
    using ElemType = function<void()>;
public:
    TaskQueue(size_t);
    ~TaskQueue();
    bool empty() const;
    bool full() const;
    void push(ElemType &&);
    ElemType pop();
    void wakeup();

private:
    size_t _queSize;
    queue<ElemType> _que;
    MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;
    bool _flag; // 线程是否在
};

#endif
