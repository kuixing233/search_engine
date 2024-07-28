#ifndef __KEYRECOMMANDER_H__
#define __KEYRECOMMANDER_H__

#include <string>
#include <queue>
#include <set>
#include <vector>
#include <unordered_map>

#include "Singleton.h"

class CandidateResult
{
    friend bool operator<(const CandidateResult &lhs, const CandidateResult &rhs);

public:
    CandidateResult(const std::string &word, int freq, int dist)
        : _word(word), _freq(freq), _dist(dist)
    {
    }

public:
    std::string _word;
    int _freq;
    int _dist;
};

class KeyRecommander
    : public Singleton<KeyRecommander>
{
    friend Singleton<KeyRecommander>;
public:
    std::string doQuery(std::string queryWord); // 执行查询

private:
    void loadLibray();
    bool queryIndextable(std::string queryWord, std::priority_queue<CandidateResult> &resultQue);          // 查询索引
    std::string createRetJsonStr(std::priority_queue<CandidateResult> &resultQue); // 响应客户端的请求
    std::string returnNoAnswer();

private:
    KeyRecommander();

    std::vector<std::pair<std::string, int>> _dict;             // 词典
    std::unordered_map<std::string, std::set<int>> _index;      // 索引表
};

#endif