#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "TcpServer.h"
#include "ThreadPool.h"
#include "KeyRecommander.h"

#include <iostream>

using std::cout;
using std::endl;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }
    ~MyTask()
    {

    }
    void process()
    {
        // 要把处理完毕的msg发送给主线程EventLoop，
        // 让主线程负责发送给客户端

        
        
        // 数据的发送，由子线程交给主线程EventLoop发送
        _con->sendToLoop(_msg);
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize, 
               const string &ip, unsigned short port)
    : _pool(threadNum, queSize)
    , _tcpserver(ip, port)
    {

    }

    ~EchoServer()
    {

    }

    void start()
    {
        using namespace std::placeholders;
        _pool.start();

        _tcpserver.setAllCallback(std::bind(&EchoServer::onConnection, this, _1),
                                  std::bind(&EchoServer::onMessage, this, _1),
                                  std::bind(&EchoServer::onClose, this, _1));
        _tcpserver.start();
    }

    void stop()
    {
        _pool.stop();
        _tcpserver.stop();
    }

    void onConnection(const TcpConnectionPtr& con)
    {
       cout << con->toString() << " has connected!" << endl; 
    }
    
    void onMessage(const TcpConnectionPtr& con)
    {
        // 数据的接收
        string msg = con->receive();
        msg = msg.substr(0, msg.size() - 1);
        cout << ">>> recv message from client: " << msg << endl;
        
        // 数据的处理，在子线程中进行
    
        // MyTask task(msg, con);
        // _pool.addTask(std::bind(&MyTask::process, task));

        KeyRecommander keyReco(msg, con);
        _pool.addTask(std::bind(&KeyRecommander::execute, &keyReco));
    
        // 数据的发送
        /* con->send(msg1); */
    }
    
    void onClose(const TcpConnectionPtr& con)
    {
        cout << con->toString() << " has destroyed!" << endl;
    }

private:
    ThreadPool _pool;
    TcpServer _tcpserver;
};
#endif
