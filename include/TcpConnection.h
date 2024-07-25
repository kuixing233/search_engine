#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;

class TcpConnection;
class EventLoop;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
public:
    explicit TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg);
    string receive();

    // 将msg发送给EventLoop
    void sendToLoop(const string &msg);

    //为了方便调试的函数
    string toString();
    bool isClosed();

    // 三个回调函数的注册
    // 注意不能使用右值引用
    void setOnConnectionCallback(const TcpConnectionCallback & _cb);
    void setOnMessageCallback(const TcpConnectionCallback & _cb);
    void setOnCloseCallback(const TcpConnectionCallback & _cb);
    // 三个回调函数的执行
    void handleOnConnectionCallback();
    void handleOnMessageCallback();
    void handleOnCloseCallback();

private:
    //获取本端地址与对端地址
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private:
    SocketIO _sockIO;

    // 为了将msg发送给EventLoop，需要设置一个指针
    EventLoop * _loop;

    //为了调试而加入的函数
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;

    // Tcp网络编程的三个事件
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
    
};

#endif
