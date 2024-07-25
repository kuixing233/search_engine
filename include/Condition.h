#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "MutexLock.h"
#include "NonCopyable.h"

#include <pthread.h>

class Condition
: public NonCopyable
{
public:
    Condition(MutexLock &);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;
};

#endif
