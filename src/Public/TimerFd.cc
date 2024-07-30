#include <sys/timerfd.h>
#include <poll.h>
#include <unistd.h>

#include "TimerFd.h"

TimerFd::TimerFd(TimerFdCallback cb)
: _timerfd(createTimerfd())
, _isStarted(false)
, _cb(cb)
{

}

TimerFd::~TimerFd()
{
    setTimerfdTime(0, 0);
    close(_timerfd);
}

void TimerFd::start()
{
    _isStarted = true;

    setTimerfdTime(5, 60); // 开始后5s启动定时器，之后每60s超时一次

    struct pollfd fds;
    fds.fd = _timerfd;
    fds.events = POLLIN;

    while (_isStarted)
    {
        int nready = poll(&fds, 1, 5000);
        if (-1 == nready)
        {
            perror("poll");
            return;
        }
        else if (0 == nready)
        {
            // cout << "pool timeout!" << endl;
        }
        else
        {
            if (fds.revents & POLLIN)
            {
                handleRead();
                if (_cb)
                {
                    _cb();
                }
            }
        }
    }
}

void TimerFd::stop()
{
    _isStarted = false;
}

void TimerFd::handleRead()
{
    uint64_t one = 1;
    int ret = ::read(_timerfd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t))
    {
        perror("handleRead");
        return;
    }
}

void TimerFd::setTimerfdTime(int it_value, int it_interval)
{
    struct itimerspec new_value;
    new_value.it_value.tv_sec = it_value;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = it_interval;
    new_value.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(_timerfd, 0, &new_value, nullptr);
    if (-1 == ret)
    {
        perror("timerfd_settime");
        return;
    }
}

int TimerFd::createTimerfd()
{
    int timerfd = timerfd_create(CLOCK_REALTIME, 0);
    if (-1 == timerfd)
    {
        perror("timerfd_create");
    }
    return timerfd;
}
