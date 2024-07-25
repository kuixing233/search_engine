#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H_

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "Acceptor.h"
#include "TcpConnection.h"
#include "MutexLock.h"

#include <vector>
#include <map>
#include <memory>
#include <functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
class EventLoop
{
public:
    EventLoop(Acceptor &acceptor);

    ~EventLoop();

    // 事件开始循环
    void loop();

    // 事件不循环
    void unloop();

    // 作为中转的三个回调函数的注册
    void setOnConnectionCallback(TcpConnectionCallback && _cb);
    void setOnMessageCallback(TcpConnectionCallback && _cb);
    void setOnCloseCallback(TcpConnectionCallback && _cb);

    // 线程通信函数
    void handleRead();
    void wakeup();
    // 执行发送msg
    void sendInLoop(function<void()> &&cb);
    void doPendingsSendFunc();

private:
    // 执行epoll_wait
    void waitEpollFd();

    // 处理新的连接
    void handleNewConnection();

    // 处理消息的收发
    void handleMessage(int fd);

    // 创建epfd
    int createEpollFd();

    // 创建eventfd
    int createEvtFd();

    // 文件描述符放到epoll上监听
    void addEpollReadFd(int fd);

    // 从epoll上删除该文件描述符
    void delEpollReadFd(int fd);

private:
    int _epfd; // epoll描述符
    int _evtfd; // 线程间通信描述符
    bool _isLooping; // 循环是否进行的标志
    Acceptor & _acceptor; // 为了调用accept函数
    vector<struct epoll_event> _evtList; // 存放满足条件的事件
    map<int, TcpConnectionPtr> _conns; // TcpConnection对象语义
    vector<function<void()>> _pendsSendFunc; // 将要发送的回调函数存起来，防止多个线程同时要发送数据
    MutexLock _mutex; // 防止多个线程同时访问vector

    // 作为中转的三个回调函数
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif
