#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "EventLoop.h"
#include "Acceptor.h"

class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port);
    ~TcpServer();
    void start();
    void stop();
    void setAllCallback(TcpConnectionCallback && onConnection, 
                        TcpConnectionCallback && onMessage,
                        TcpConnectionCallback && onClose);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif
