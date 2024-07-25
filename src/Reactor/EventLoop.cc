#include <string.h>
#include <unistd.h>
#include "EventLoop.h"
#include "MutexLockGuard.h"


EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evtfd(createEvtFd())
, _isLooping(false)
, _acceptor(acceptor)
, _evtList(1024)
{
    addEpollReadFd(_acceptor.fd());
    addEpollReadFd(_evtfd);
}

EventLoop::~EventLoop()
{
    close(_epfd);
    close(_evtfd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while (_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::setOnConnectionCallback(TcpConnectionCallback && _cb)
{
    _onConnection = std::move(_cb);
}

void EventLoop::setOnMessageCallback(TcpConnectionCallback && _cb)
{
    _onMessage = std::move(_cb);
}

void EventLoop::setOnCloseCallback(TcpConnectionCallback && _cb)
{
    _onClose = std::move(_cb);
}

void EventLoop::handleRead()
{
    uint64_t one;
    int ret = read(_evtfd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t))
    {
        perror("handleRead");
        return;
    }
}

void EventLoop::wakeup()
{
    uint64_t one;
    int ret = write(_evtfd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t))
    {
        perror("wakeup");
        return;
    }
}

void EventLoop::sendInLoop(function<void()> &&cb)
{
    {
        MutexLockGuard autoLock(_mutex);
        _pendsSendFunc.push_back(std::move(cb));
    }
    wakeup(); 
}

void EventLoop::doPendingsSendFunc()
{
    vector<function<void()>> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendsSendFunc);
    }

    for (auto &cb : tmp)
    {
        if (cb)
        {
            cb();
        }
    }
}

void EventLoop::waitEpollFd()
{
    int nready;
    do
    {
        nready = ::epoll_wait(_epfd, _evtList.data(), _evtList.size(), 3000);    
    }
    while(-1 == nready && errno == EINTR);

    if (-1 == nready)
    {
        perror("epoll_wait");
        return;
    }
    else if (0 == nready)
    {
        printf(">> epoll_wait timeout!\n");
    }
    else
    {
        // 防止连接数超过_evtList容量，需要进行扩容
        if (nready == (int)_evtList.size())
        {
            _evtList.resize(2 * nready);
        }

        for (int idx = 0; idx != nready; ++ idx)
        {
            int fd = _evtList[idx].data.fd;
            if (fd == _acceptor.fd())
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    handleNewConnection();
                }
            }
            else if (fd == _evtfd)
            {
                if (_evtList[idx].events & EPOLLIN)
                {
                    handleRead();
                    doPendingsSendFunc();
                }
            }
            else
            {
                if (_evtList[idx].events & EPOLLIN)
                {
                    handleMessage(fd);
                }
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerfd = _acceptor.accept();
    if (peerfd < 0)
    {
        perror("handleNewConnection\n");
        return;
    }
    addEpollReadFd(peerfd);

    // 建立连接
    TcpConnectionPtr con(new TcpConnection(peerfd, this));
    // 注册回调函数
    // 因为每建立一次新连接就要注册一次，
    // 不能使用右值引用
    con->setOnConnectionCallback(_onConnection);
    con->setOnMessageCallback(_onMessage);
    con->setOnCloseCallback(_onClose);

    _conns.insert(std::make_pair(peerfd, con));

    // 执行回调函数
    con->handleOnConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    if (it != _conns.end())
    {
        if (it->second->isClosed())
        {
            it->second->handleOnCloseCallback();
            delEpollReadFd(fd);
            _conns.erase(it);
        }
        else
        {
            it->second->handleOnMessageCallback();
        }
    }
}

int EventLoop::createEpollFd()
{
    int epfd = ::epoll_create1(0);
    if (epfd < 0)
    {
        perror("epfd");
    }
    return epfd;
}

int EventLoop::createEvtFd()
{
    int evtfd = ::eventfd(10, 0);
    if (evtfd < 0)
    {
        perror("evtfd");
    }
    return evtfd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
    ev.data.fd = fd;
    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
    if (ret < 0)
    {
        perror("addEpollReadFd");
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
    ev.data.fd = fd;
    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &ev);
    if (ret < 0)
    {
        perror("delEpollReadFd");
    }
}
