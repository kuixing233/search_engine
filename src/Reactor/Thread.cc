#include "Thread.h"

__thread const char * name = "default thread name";

Thread::Thread(ThreadCallback &&cb, const std::string & name = std::string())
: _thid(0)
, _name(name)
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
    std::cout << "pthread_create success, name = " << _name << std::endl;
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
    name = pth1->_name.c_str(); // 当前线程的局部变量name就被修改了
    if (pth1)
        pth1->_cb();
    pthread_exit(nullptr);
}

