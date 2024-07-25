#include "KeyRecommander.h"
#include "LogMgr.h"

#include <algorithm>
#include <iostream>
// #include "json.hpp"
#include <json/json.h>

bool operator < (const CandidateResult& lhs, const CandidateResult& rhs)
{
    if (lhs._dist < rhs._dist) {
        return false;
    } else if (lhs._dist == rhs._dist) {
        if (lhs._freq < rhs._freq) {
            return false;
        } else if (lhs._freq == rhs._freq) {
            if (lhs._word < rhs._word) {
                return false;
            }
        }
    }
    return true;
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

KeyRecommander::KeyRecommander(const std::string & query, const TcpConnectionPtr & conn)
    : _queryWord(query)
    , _conn(conn)
{
}

KeyRecommander::KeyRecommander(const std::string & query)
    : _queryWord(query)
    , _conn(nullptr)
{
}

void KeyRecommander::execute()
{
    queryIndextable();
    response();
}

void KeyRecommander::queryIndextable()
{
    auto dict = Dictionary::GetInstance()->getDict();
    auto indexTable = Dictionary::GetInstance()->getIndexTable();

    std::vector<std::string> wordList;
    for (std::size_t i = 0; i < _queryWord.size();) {
        std::size_t nBytes = nBytesCode(_queryWord[i]);
        wordList.push_back(_queryWord.substr(i, nBytes));
        i += nBytes;
    }

    std::set<int> _cond_word_index;
    for (auto & word : wordList) {
        std::cout << "字符 [" << word << "] ";
        std::set<int> indexSet = indexTable[word];
        if (indexSet.size() > 0) {
            std::cout << "index size: " << indexSet.size() << std::endl;
            _cond_word_index.insert(indexSet.begin(), indexSet.end());
        }
        else
            std::cout << "index zero" << std::endl;
    }

    for (auto & index : _cond_word_index) {
        std::pair<std::string, int> dictPair = dict[index];
        std::string w = dictPair.first;
        int f = dictPair.second;
        int d = distance(w);
        std::cout << "index: " << index << " " << "word: " << w << ": " << f << "、" << d << std::endl;
        _resultQue.emplace(CandidateResult(dict[index].first, dict[index].second, distance(dict[index].first)));
    }
}

int KeyRecommander::distance(const std::string &rhs)
{
    return editDistance(this->_queryWord, rhs);
}

void KeyRecommander::response()
{
    // using Json = nlohmann::json;
    // Json retJson;
    Json::Value retJson;
    int max_index = _resultQue.size() > 5 ? 5 : _resultQue.size();
    std::cout << "候选词个数：" <<_resultQue.size() << std::endl;
    for (int i = 0; i < max_index; ++ i) {
        std::cout << _resultQue.top()._word << ": " << _resultQue.top()._dist << "、" << _resultQue.top()._freq << std::endl;
        CandidateResult res = _resultQue.top();
        // Json value;
        Json::Value value;
        value["word"] = res._word;
        value["dist"] = res._dist;
        value["freq"] = res._freq;
        // retJson.push_back(value);
        retJson.append(value);
        _resultQue.pop();
    }

    // std::string retJsonStr = retJson.dump();
    std::string retJsonStr = retJson.toStyledString();
    LogInfo("retJsonStr: %s", retJsonStr.c_str());
    // _conn->sendToLoop(retJsonStr);
}