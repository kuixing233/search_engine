#include "Thread.h"

Thread::Thread(ThreadCallback &&cb)
: _thid(0)
, _isRunning(false)
, _cb(std::move(cb))
{
}

Thread::~Thread()
{
    if (_isRunning)
    {
        pthread_detach(_thid);
        _isRunning = false;
    }
}

void Thread::start()
{
    int ret = pthread_create(&_thid, nullptr, threadFunc, this); 
    if (ret)
    {
        perror("pthread_create");
        return;
    }
    _isRunning = true;
}

void Thread::join()
{
    int ret = pthread_join(_thid, nullptr); 
    if (ret)
    {
        perror("pthread_join");
        return;
    }
    _isRunning = false;
}

void * Thread::threadFunc(void *args)
{
    Thread *pth1 = (Thread *)args;
    if (pth1)
        pth1->_cb();
    pthread_exit(nullptr);
}

