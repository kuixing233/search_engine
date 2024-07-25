#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include "NonCopyable.h"

#include <pthread.h>


class MutexLock
: public NonCopyable // 对象语义，不能复制或者赋值
{
public:
    MutexLock();
    ~MutexLock();
    void lock();
    void trylock();
    void unlock();

    pthread_mutex_t * getMutexPtr()
    {
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;
};

#endif
