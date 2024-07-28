#include "MySimHash.h"
#include "ConfigMgr.h"
#include "StopWords.h"
#include "SplitToolCppJieba.h"
#include <unordered_map>
#include <algorithm>

MySimHash::MySimHash()
    : _simHasher(ConfigMgr::Inst()["CppJieba"]["DICT_PATH"],
                 ConfigMgr::Inst()["CppJieba"]["HMM_PATH"],
                 ConfigMgr::Inst()["CppJieba"]["IDF_PATH"],
                 ConfigMgr::Inst()["CppJieba"]["STOP_WORD_PATH"])
{
}

MySimHash::~MySimHash()
{
}

uint64_t MySimHash::getSimHashValue(const std::string &str, size_t k)
{
    uint64_t _u64;
    _simHasher.make(str, k, _u64);
    return _u64;
}

std::vector<std::pair<std::string, double>> MySimHash::getTopK(const std::string &str, size_t k)
{
    std::vector<std::pair<std::string, double>> topKVec;
    _simHasher.extract(str, topKVec, k);
    return topKVec;
}

std::unordered_map<std::string, int> MySimHash::getWordFrec(const std::string &str)
{
    std::unordered_map<std::string, int> wordFreq;
    const auto &stopWordList = StopWords::GetInstance()->getStopWordList();
    std::vector<std::string> wordList = SplitToolCppJieba::GetInstance()->cut(str);
    for (auto it = wordList.begin(); it != wordList.end();)
    {
        if (stopWordList.find(*it) != stopWordList.end())
        {
            wordList.erase(it);
        }
        else
        {
            ++it;
        }
    }
    for (auto word : wordList)
    {
        unsigned char tmp = word[0];
        if (isalpha(tmp))
        {
            std::transform(word.begin(), word.end(), word.begin(), tolower);
        }
        else if (tmp < 128)
        {
            continue;
        }

        ++wordFreq[word];
    }
    return wordFreq;
}