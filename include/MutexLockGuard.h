#ifndef __MUTEXLOCKGUARD_H__
#define  __MUTEXLOCKGUARD_H__

#include "MutexLock.h"

// 使用RAII思想，防止发生死锁现象
class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock & mutex)
    : _mutex(mutex)
    {
        _mutex.lock();
    }

    ~MutexLockGuard()
    {
        _mutex.unlock();
    }

private:
    MutexLock & _mutex;
};

#endif
