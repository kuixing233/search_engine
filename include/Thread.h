#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <functional>

using std::cout;
using std::endl;
using std::function;

#define ERROR_CHECK(msg, ret) \
    do {    \
        printf("%s: %s", msg, strerror(ret));\
    } while (0)

extern __thread const char * name;

class Thread
{
public:
    using ThreadCallback = function<void()>;
public:
    Thread(ThreadCallback &&, const std::string &);
    ~Thread();

    // 线程创建和等待函数
    void start();
    void join();

    // 线程执行函数
private:
    static void * threadFunc(void *);
    void run();

private:
    pthread_t _thid;
    std::string _name;
    bool _isRunning;
    ThreadCallback _cb;
};

#endif
