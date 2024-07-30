#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <json/json.h>

#include <iostream>
#include <string>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::string;

/* #define ERROR_CHECK(msg, ret) if (ret == -1) {printf("%s error!\n", msg);} */

//len = 10000    1500/6     1000/1
int readn(int _fd, char *buf, int len)
{
    int left = len;
    char *pstr = buf;
    int ret = 0;

    while(left > 0)
    {
        ret = read(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("read error -1");
            return len - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            pstr += ret;
            left -= ret;
        }
    }

    return len - left;
}

int readLine(int _fd, char *buf, int len)
{
    cout << "开始从服务端接收数据" << endl;
    int left = len - 1;
    char *pstr = buf;
    int ret = 0, total = 0;

    while(left > 0)
    {
        //MSG_PEEK不会将缓冲区中的数据进行清空,只会进行拷贝操作
        ret = recv(_fd, pstr, left, MSG_PEEK);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("readLine error -1");
            return len - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            for(int idx = 0; idx < ret; ++idx)
            {
                if(pstr[idx] == '\n')
                {
                    int sz = idx + 1;
                    readn(_fd, pstr, sz);
                    pstr += sz;
                    *pstr = '\0';//C风格字符串以'\0'结尾
                    return total + sz;
                }
            }

            readn(_fd, pstr, ret);//从内核态拷贝到用户态
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }
    *pstr = '\0';

    return total - left;
}

int writen(int _fd, const char *buf, int len)
{
    int left = len;
    const char *pstr = buf;
    int ret = 0;

    while(left > 0)
    {
        ret = write(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("writen error -1");
            return len - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            pstr += ret;
            left -= ret;
        }
    }

    return len - left;
}


void keyRecPro(string retJsonStr)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(retJsonStr, root);
    int code = root["code"].asInt();
    if (code != 0)
    {
        cout << ">> system: 未找到相关内容！" << endl;
        return;
    }
    Json::Value data = root["data"];
    cout << ">> system: 关键词查询结果如下：" << endl;
    for (int i = 0; i < data.size(); ++i)
    {
        cout << data[i]["word"].asString() << endl;
    }
    std::cout << endl;
}

void webQueryPro(string retJsonStr)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(retJsonStr, root);
    int code = root["code"].asInt();
    if (code != 0)
    {
        cout << ">> system: 未找到相关内容！" << endl;
        return;
    }
    Json::Value data = root["data"];
    cout << ">> system: 网页查询结果如下：" << endl;
    for (int i = 0; i < data.size(); ++i)
    {
        cout << "[标题]：" << data[i]["title"].asString() << endl;
        cout << "[链接]：" << data[i]["url"].asString() << endl;
        cout << "[简介]：" << data[i]["desc"].asString() << endl;
        cout << endl;
    }
}

void test()
{
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0)
    {
        perror("socket");
        return;
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8888);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.25.131");
    int ret = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0)
    {
        perror("connect");
        close(clientfd);
        return;
    }

    char resp[65535] = {0};
    string reqStr;
    while (1)
    {
        int choice;
        cout << ">> 请输入选择（1. 关键字查询， 2. 网页查询）：" << endl;
        scanf("%d", &choice);
        if (choice != 1 && choice != 2)
        {
            cout << ">> system: 输入有误，请重新输入！" << endl;
            continue;
        }
        getchar(); // 读取缓冲区中的换行符
        cout << ">> 请输入想要查询的内容：" << endl;
        string tmp;
        std::getline(cin, tmp);
        reqStr = std::to_string(choice) + " " + tmp + "\n";
        writen(clientfd, reqStr.c_str(), reqStr.size());
        reqStr.clear();
        cout << "正在等待服务器的响应。。。" << endl;

        bzero(resp, sizeof(resp));
        readLine(clientfd, resp, sizeof(resp));

        switch(choice)
        {
            case 1:
                keyRecPro(resp);
                break;
            case 2:
                webQueryPro(resp);
                break;
            default:
                break;
        }
        // cout << ">> client receive: " << resp << endl;
    }
    close(clientfd);
}

int main(int argc, char **argv)
{
    test();
    return 0;
}