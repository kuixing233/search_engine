#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, unsigned short port)
: _acceptor(ip, port)
, _loop(_acceptor)
{
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    _acceptor.ready();
    _loop.loop();
}

void TcpServer::stop()
{
    _loop.unloop();
}

void TcpServer::setAllCallback(TcpConnectionCallback && onConnection, 
                        TcpConnectionCallback && onMessage,
                        TcpConnectionCallback && onClose)
{
    _loop.setOnConnectionCallback(std::move(onConnection));
    _loop.setOnMessageCallback(std::move(onMessage));
    _loop.setOnCloseCallback(std::move(onClose));
}

