#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <sstream>


using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd, EventLoop *loop)
: _sockIO(fd)
, _loop(loop)
, _sock(fd)
, _localAddr(getLocalAddr())
, _peerAddr(getPeerAddr())
{

}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::send(const string &msg)
{
    _sockIO.writen(msg.c_str(), msg.size());
}

string TcpConnection::receive()
{
    char buff[65535] = {0};
    _sockIO.readLine(buff, sizeof(buff));

    return string(buff);
}

void TcpConnection::sendToLoop(const string &msg)
{
    _loop->sendInLoop(std::bind(&TcpConnection::send, this, msg));
}

string TcpConnection::toString()
{
    ostringstream oss;
    oss << _localAddr.ip() << ":"
        << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();

    return oss.str();
}

bool TcpConnection::isClosed()
{
    char buff[128] = {0};
    int ret = recv(_sock.fd(), buff, sizeof(buff), MSG_PEEK);
    return ret == 0;
}

//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr );
    //获取本端地址的函数getsockname
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
    if(-1 == ret)
    {
        perror("getsockname");
    }

    return InetAddress(addr);
}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr );
    //获取对端地址的函数getpeername
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
    if(-1 == ret)
    {
        perror("getpeername");
    }

    return InetAddress(addr);
}

void TcpConnection::setOnConnectionCallback(const TcpConnectionCallback & cb)
{
    _onConnection = cb;
}

void TcpConnection::setOnMessageCallback(const TcpConnectionCallback & cb)
{
    _onMessage = cb;
}

void TcpConnection::setOnCloseCallback(const TcpConnectionCallback & cb)
{
    _onClose = cb;
}

void TcpConnection::handleOnConnectionCallback()
{
    if (_onConnection)
        _onConnection(shared_from_this());
}

void TcpConnection::handleOnMessageCallback()
{
    if (_onMessage)
        _onMessage(shared_from_this());
}

void TcpConnection::handleOnCloseCallback()
{
    if (_onClose)
        _onClose(shared_from_this());
}
