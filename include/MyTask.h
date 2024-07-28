#ifndef __KX_MYTASK_H__
#define __KX_MYTASK_H__

#include "TcpConnection.h"
#include "KeyRecommander.h"
#include "WebPageQuery.h"

#include <json/json.h>

class MyTask
{
public:
    MyTask(const TcpConnectionPtr &con, const string &msg)
    : _con(con)
    , _msg(msg)
    {

    }
    ~MyTask()
    {

    }
    void process()
    {
        int choice = atoi(_msg.substr(0, 1).c_str());
        std::string resStr;
        switch (choice)
        {
        case 1:
            resStr = KeyRecommander::GetInstance()->doQuery(_msg.substr(2));
            break;
        case 2:
            resStr = WebPageQuery::GetInstance()->doQuery(_msg.substr(2));
            break;
        default:
            break;
        };
        _con->sendToLoop(resStr);
    }

private:
    TcpConnectionPtr _con;
    string _msg;
};

#endif