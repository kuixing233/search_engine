#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "TcpServer.h"
#include "ThreadPool.h"
#include "MyTask.h"
#include "LogMgr.h"

#include <iostream>

using std::cout;
using std::endl;

class SearchEngine
{
public:
    SearchEngine(size_t threadNum, size_t queSize,
                 const string &ip, unsigned short port)
        : _pool(threadNum, queSize), _tcpserver(ip, port), _tfd((std::bind(&CacheManager::periodicUpdateCaches, CacheManager::GetInstance())))
    {
        LogInfo("SearchEngine Start on IP: %s Port: %d", ip.c_str(), port);
    }

    ~SearchEngine()
    {
    }

    void start()
    {
        using namespace std::placeholders;
        _pool.start();

        // Thread th(std::bind(&TimerFd::start, &_tfd), "缓存更新线程");
        // th.start();

        _pool.addTask(std::bind(&TimerFd::start, &_tfd));

        _tcpserver.setAllCallback(std::bind(&SearchEngine::onConnection, this, _1),
                                  std::bind(&SearchEngine::onMessage, this, _1),
                                  std::bind(&SearchEngine::onClose, this, _1));
        _tcpserver.start();
    }

    void stop()
    {
        _pool.stop();
        _tcpserver.stop();
    }

    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }

    void onMessage(const TcpConnectionPtr &con)
    {
        // 数据的接收，放到子线程中
        string msg = con->receive();
        msg = msg.substr(0, msg.size() - 1);
        cout << ">>> recv message from client: " << msg << endl;

        // 数据的处理
        MyTask task(con, msg);
        _pool.addTask(std::bind(&MyTask::process, task));
    }

    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has destroyed!" << endl;
    }

private:
    ThreadPool _pool;
    TcpServer _tcpserver;
    TimerFd _tfd;
};
#endif
