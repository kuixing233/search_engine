#include "KeyRecommander.h"
#include "LogMgr.h"
#include "ConfigMgr.h"

#include "json.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>

bool operator<(const CandidateResult &lhs, const CandidateResult &rhs)
{
    // 优先队列默认大根堆，使用std::less，排序结果是从大到小
    // 编辑距离从小到大排序
    if (lhs._dist != rhs._dist)
    {
        return lhs._dist > rhs._dist; // 编辑距离越大排在越后面
    }
    // 编辑距离相同，按词频从大到小排序
    else if (lhs._freq != rhs._freq)
    {
        return lhs._freq < rhs._freq; // 频次越小排在越后面
    }
    // 编辑距离和词频相同，按字典序从小到大排序
    return lhs._word > rhs._word; // 字典序越大排在越后面
}

/************************最小编辑距离算法 start************************/
// 1. 求取一个字符占据的字节数
size_t nBytesCode(const char ch)
{
    if (ch & (1 << 7))
    {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx)
        {
            if (ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

// 2. 求取一个字符串的字符长度
std::size_t length(const std::string &str)
{
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }

    return ilen;
}

int triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

// 3. 中英文通用的最小编辑距离算法
int editDistance(const std::string &lhs, const std::string &rhs)
{
    // 计算最小编辑距离-包括处理中英文
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }
    for (size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }
    std::string sublhs, subrhs;
    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,
                     ++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for (std::size_t dist_j = 1, rhs_idx = 0;
             dist_j <= rhs_len; ++dist_j, ++rhs_idx)
        {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j -
                                                                1];
            }
            else
            {
                editDist[dist_i][dist_j] =
                    triple_min(editDist[dist_i][dist_j - 1] + 1,
                               editDist[dist_i - 1][dist_j] + 1,
                               editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}
/************************最小编辑距离算法 end************************/

KeyRecommander::KeyRecommander()
{
    loadLibray();
}

std::string KeyRecommander::doQuery(std::string queryWord)
{
    std::string retJsonStr;

    // if (RedisMgr::GetInstance()->ExistsKey(queryWord))
    // {
    //     LogInfo("查询词 [%s] Redis缓存命中，从Redis中获取数据", queryWord.c_str());
    //     RedisMgr::GetInstance()->Get(queryWord, retJsonStr);
    //     return retJsonStr;
    // }
    std::priority_queue<CandidateResult> resultQue;
    bool ret = queryIndextable(queryWord, resultQue);
    if (ret == false)
    {
        retJsonStr = returnNoAnswer();
        return retJsonStr;
    }
    retJsonStr = createRetJsonStr(resultQue);
    // RedisMgr::GetInstance()->Set(queryWord, retJsonStr);
    // LogInfo("查询词 [%s] 缓存未命中，保存到Redis缓存", queryWord.c_str());
    return retJsonStr;
}

bool KeyRecommander::queryIndextable(std::string queryWord, std::priority_queue<CandidateResult> &resultQue)
{
    // 将查询词分割成单个字
    std::vector<std::string> wordList;
    for (std::size_t i = 0; i < queryWord.size();)
    {
        std::size_t nBytes = nBytesCode(queryWord[i]);
        wordList.push_back(queryWord.substr(i, nBytes));
        i += nBytes;
    }

    // 查询索引表，将所有单个字的索引合并，去重
    std::set<int> _condWordIndex;
    for (auto &word : wordList)
    {
        // std::cout << "字 [" << word << "] ";
        std::set<int> indexSet = _index[word];
        if (indexSet.size() > 0)
        {
            // std::cout << "index size: " << indexSet.size() << std::endl;
            _condWordIndex.insert(indexSet.begin(), indexSet.end());
        }
    }

    if (_condWordIndex.empty())
    {
        return false;
    }

    // 对索引表中的词进行排序
    for (auto &index : _condWordIndex)
    {
        std::pair<std::string, int> dictPair = _dict[index];
        std::string w = dictPair.first;
        int f = dictPair.second;
        int d = editDistance(queryWord, w);
        // std::cout << "index: " << index << " " << "word: " << w << ": freq: " << f << "、dist: " << d << std::endl;
        resultQue.emplace(CandidateResult(w, f, d));
    }
    return true;
}

std::string KeyRecommander::createRetJsonStr(std::priority_queue<CandidateResult> &resultQue)
{
    using Json = nlohmann::json;
    Json retJson;
    retJson["code"] = 0;
    int max_index = resultQue.size() > 5 ? 5 : resultQue.size();
    std::cout << "候选词个数：" << resultQue.size() << std::endl;
    for (int i = 0; i < max_index; ++i)
    {
        // std::cout << resultQue.top()._word << ": " << resultQue.top()._dist << "、" << resultQue.top()._freq << std::endl;
        CandidateResult res = resultQue.top();
        Json value;
        value["word"] = res._word;
        value["dist"] = res._dist;
        value["freq"] = res._freq;
        retJson["data"].push_back(value);
        resultQue.pop();
    }
    std::string retJsonStr = retJson.dump();
    LogInfo("匹配到结果: %s", retJsonStr.c_str());
    return retJsonStr;
}

std::string KeyRecommander::returnNoAnswer()
{
    using Json = nlohmann::json;
    Json retJson;
    retJson["code"] = 1;
    retJson["data"] = "未查询到结果";
    std::string retJsonStr = retJson.dump();
    LogInfo("未查询到结果: %s", retJsonStr.c_str());
    return retJsonStr;
}

void KeyRecommander::loadLibray()
{
    std::ifstream ifdDict(ConfigMgr::Inst()["Key"]["dict"]);
    if (!ifdDict.is_open())
    {
        LogError("open file %s error!", ConfigMgr::Inst()["Key"]["dict"].c_str());
    }

    std::string line;
    std::string word;
    int frequency;
    while (getline(ifdDict, line))
    {
        std::istringstream iss(line);
        iss >> word >> frequency;
        _dict.push_back(std::make_pair(word, frequency));
    }
    ifdDict.close();
    LogInfo("加载词频库成功，大小: %d", _dict.size());

    std::ifstream ifdIndex(ConfigMgr::Inst()["Key"]["index"]);
    if (!ifdIndex.is_open())
    {
        LogError("open file %s error!", ConfigMgr::Inst()["Key"]["index"].c_str());
    }
    int index;
    while (getline(ifdIndex, line))
    {
        std::istringstream iss(line);
        iss >> word;
        while (iss >> index)
        {
            _index[word].insert(index);
        }
    }
    ifdIndex.close();
    LogInfo("加载词典索引库成功，大小: %d", _index.size());
}