#ifndef __TIMERFD_H__
#define __TIMERFD_H__

#include <functional>
#include <iostream>

using std::function;
using std::cout;
using std::endl;

class TimerFd
{
    using TimerFdCallback = function<void()>;
public:
    TimerFd(TimerFdCallback cb);
    ~TimerFd();

    void start();
    void stop();

private:
    void handleRead();
    void setTimerfdTime(int it_value, int it_interval);
    int createTimerfd();

private:
    int _timerfd;
    bool _isStarted;
    TimerFdCallback _cb;
};
#endif
