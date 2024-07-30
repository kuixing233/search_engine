#ifndef __KX_MYTASK_H__
#define __KX_MYTASK_H__

#include "TcpConnection.h"
#include "KeyRecommander.h"
#include "WebPageQuery.h"
#include "CacheManager.h"

#include <json/json.h>

class MyTask
{
public:
    MyTask(const TcpConnectionPtr &con, const string &msg)
        : _con(con), _msg(msg)
    {
    }
    ~MyTask()
    {
    }
    void process()
    {

        cout << "线程 [" << name << "] 为您服务" << endl;
        int choice = atoi(_msg.substr(0, 1).c_str());
        string msg = _msg.substr(2);
        std::string resStr;
        bool bCache = CacheManager::GetInstance()->getCache(atoi(name)).getElement(_msg, resStr);
        if (!bCache)
        {
            std::cout << "缓存未命中" << std::endl;
            switch (choice)
            {
            case 1:
                resStr = KeyRecommander::GetInstance()->doQuery(msg);
                break;
            case 2:
                resStr = WebPageQuery::GetInstance()->doQuery(msg);
                break;
            default:
                break;
            };
            if (resStr[resStr.size() - 1] == '\n')
            {
                std::cout << "Json FastWriter得到的结果是有换行的" << std::endl;
                resStr = resStr.substr(0, resStr.size() - 1);
            }
 
            std::cout << "将结果写入缓存" << std::endl;
            CacheManager::GetInstance()->getCache(atoi(name)).addElement(_msg, resStr);
            cout << "线程 [" << name << "] 当前缓存如下：" << endl;
            CacheManager::GetInstance()->getCache(atoi(name)).getCache();
        }
        else
        {
            std::cout << "缓存命中" << std::endl;
        }
        if (resStr[resStr.size() - 1] != '\n')
        {
            std::cout << "没有换行，resStr.size() = " << resStr.size() << std::endl;
            resStr += "\n";
        }
        else
        {
            std::cout << "有换行，直接发送" << std::endl;
            resStr[resStr.size() - 1] = '\n';
        }
        std::cout << "服务器发送的信息大小：size() = " << resStr.size() << std::endl;
        _con->sendToLoop(resStr);
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};

#endif