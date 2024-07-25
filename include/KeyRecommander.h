#ifndef __KEYRECOMMANDER_H__
#define __KEYRECOMMANDER_H__

#include <string>
#include <queue>
#include <set>
#include <vector>

#include "Dictionary.h"
#include "TcpConnection.h"

class CandidateResult 
{
    friend bool operator < (const CandidateResult& lhs, const CandidateResult& rhs);
public:
    CandidateResult(const std::string& word, int freq, int dist)
        : _word(word)
        , _freq(freq)
        , _dist(dist)
    {

    }

public:
    std::string _word;
    int _freq;
    int _dist;
};

class KeyRecommander
{
public:
    KeyRecommander(const std::string & query, const TcpConnectionPtr & conn);
    KeyRecommander(const std::string & query);
    void execute();                                 // 执行查询

private:
    void response();                                // 响应客户端的请求
    void queryIndextable();                         // 查询索引
    int distance(const std::string & rhs);          // 计算最小编辑距离

private:
    std::string _queryWord;                                                     // 等待查询的单词
    TcpConnectionPtr _conn;                                                     // 与客户端进行连接的文件描述符
    std::priority_queue<CandidateResult, std::vector<CandidateResult>> _resultQue;      // 保存候选结果集的优先队列
};

#endif