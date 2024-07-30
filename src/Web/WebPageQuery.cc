#include "WebPageQuery.h"
#include "MySimHash.h"
#include "LogMgr.h"
#include "ConfigMgr.h"
#include "StopWords.h"
#include "RssParse.h"

#include <json/json.h>

#include <algorithm>

bool operator<(const CandidateDoc &lhs, const CandidateDoc &rhs)
{
    return lhs._cosValue > rhs._cosValue;
}

WebPageQuery::WebPageQuery()
{
    loadLibray();
}

std::string WebPageQuery::doQuery(const std::string &str)
{
    std::cout << "查询语句：" << str << std::endl;
    // 给查询字符串分词，得到词频
    std::unordered_map<std::string, int> queryWordsFreq = MySimHash::GetInstance()->getWordFrec(str);
    std::vector<std::string> queryWords;
    // 去除停用词
    auto stopWords = StopWords::GetInstance()->getStopWordList();
    for (auto it = queryWordsFreq.begin(); it != queryWordsFreq.end();)
    {
        if (stopWords.find(it->first) != stopWords.end())
        {
            queryWordsFreq.erase(it);
        }
        else
        {
            queryWords.push_back(it->first);
            ++it;
        }
    }
    // 得到每个关键词的基准向量
    std::vector<double> base = getQueryWordsWeightVector(queryWordsFreq);
    // 得到每个关键词在每个文档中的权重向量
    std::vector<std::pair<int, std::vector<double>>> resultVec;
    bool ret = executeQuery(queryWords, resultVec);
    if (ret == false)
    {
        return returnNoAnswer();
    }
    // 计算基准向量和文档向量的余弦值
    std::map<int, double> cosSimilarity = getCosSimilarity(resultVec, base);
    // 把结果存入一个优先序列
    std::priority_queue<CandidateDoc> resultQue = getResultQue(cosSimilarity, queryWords);
    return createRetJsonStr(resultQue, queryWords);
}

std::vector<double> WebPageQuery::getQueryWordsWeightVector(std::unordered_map<std::string, int> &queryWordsFreq)
{
    std::vector<double> base;
    // 将每个词看成单词，将整个句子看成文档，用TF-IDF算权重
    double N = _offsetLib.size();
    double wSum = 0;
    for (auto &pair : queryWordsFreq)
    {
        std::string word = pair.first;
        int TF = pair.second;                       // 词频，某个查询词在查询语句中出现的次数
        double DF = _invertIndexTable[word].size(); // 出现某个查询词的文章个数
        double IDF = log2(N / (DF + 1));            // 逆文档频率
        double w = TF * IDF;                        // 查询词的权重
        wSum += pow(w, 2);
        base.push_back(w); // 加入到基准向量
    }
    for (size_t i = 0; i < base.size(); ++i)
    {
        base[i] = base[i] / sqrt(wSum); // 归一化
    }
#if 0
    LogInfo("已计算基准向量");

    printf("%10s ", "keywords");
    for (auto &pair : queryWordsFreq)
    {
        printf("%10s ", pair.first.c_str());
    }
    std::cout << "\n";

    printf("%10s ", "base");
    for (auto &w : base)
    {
        printf("%10lf ", w);
    }
    std::cout << "\n";
#endif
    return base;
}

bool WebPageQuery::executeQuery(const std::vector<std::string> &queryWords, std::vector<std::pair<int, std::vector<double>>> &resultVec)
{
    // 倒排索引表中缺失查询词，直接返回
    for (const auto &word : queryWords)
    {
        if (_invertIndexTable.find(word) == _invertIndexTable.end())
        {
            return false;
        }
    }

    // 查倒排索引，取出每个单词的集合，取交集
    // 这样查到的文章就是包含所有查询词的文章
    std::set<int> docIdSet;
    for (auto &pair : _invertIndexTable[queryWords[0]])
    {
        docIdSet.insert(pair.first);
    }

    for (const auto &word : queryWords)
    {
        std::set<int> tmpIdSet;
        for (auto &pair : _invertIndexTable[word])
        {
            if (docIdSet.find(pair.first) != docIdSet.end())
            {
                tmpIdSet.insert(pair.first);
            }
        }
        docIdSet = std::move(tmpIdSet);
    }
    // 如果没有包含全部查询词的文章，直接返回
    if (docIdSet.empty())
    {
        return false;
    }

    // 找出每个关键词在每篇文章中的权重
    std::map<int, std::vector<double>> docWeight;
    for (auto &word : queryWords)
    {
        for (auto &pair : _invertIndexTable[word])
        {
            if (docIdSet.find(pair.first) != docIdSet.end())
            {
                docWeight[pair.first].push_back(pair.second);
            }
        }
    }

    for (auto &pair : docWeight)
    {
        resultVec.push_back(pair);
        // printf("%10d ", pair.first);
        // for (auto &w : pair.second)
        // {
        //     printf("%8lf ", w);
        // }
        // std::cout << "\n";
    }
    // std::cout << "docIdSet size = " << docIdSet.size() << std::endl;
    return true;
}

std::map<int, double> WebPageQuery::getCosSimilarity(const std::vector<std::pair<int, std::vector<double>>> &resultVec, const std::vector<double> &base)
{
    std::map<int, double> cosSimilarity;
    for (auto &pair : resultVec)
    {
        double numerator = 0.0;
        double base_norm = 0.0;
        double doc_norm = 0.0;
        for (size_t i = 0; i < base.size(); ++i)
        {
            numerator += base[i] * pair.second[i];
            base_norm += base[i] * base[i];
            doc_norm += pair.second[i] * pair.second[i];
        }
        double denominator = sqrt(base_norm) * sqrt(doc_norm);
        if (denominator != 0)
        {
            cosSimilarity[pair.first] = numerator / denominator;
        }
        else
        {
            cosSimilarity[pair.first] = 0.0; // 或者其他适当的默认值
        }
        // printf("%10d cosValue: %10lf\n", pair.first, cosSimilarity[pair.first]);
    }
    return cosSimilarity;
}

std::priority_queue<CandidateDoc> WebPageQuery::getResultQue(const std::map<int, double> &cosSimilarity, const std::vector<std::string> &queryWords)
{
    std::priority_queue<CandidateDoc> resultQue;
    for (auto &pair : cosSimilarity)
    {
        int docId = pair.first;
        double cosValue = pair.second;

        auto offsetPair = _offsetLib[docId];
        size_t start = offsetPair.first;
        size_t length = offsetPair.second;
        std::ifstream ifs(ConfigMgr::Inst()["Web"]["newripepage"]);
        if (!ifs.good())
        {
            LogError("open %s error", ConfigMgr::Inst()["Web"]["newripepage"].c_str());
        }
        char *buf = new char[length + 1]{0};
        ifs.seekg(start, std::ios::beg);
        ifs.read(buf, length);
        std::string docData(buf);
        RssItem docRss = RssParse::GetInstance()->parseStr(docData, queryWords)[0];
        CandidateDoc canDoc(docId, docRss._title, docRss._url, docRss._desc, cosValue);
        resultQue.push(canDoc);

        // std::cout << "docId: " << docId
        //           << " title: " << docRss._title
        //           << " url: " << docRss._url
        //           << " desc: " << docRss._desc
        //           << " cosValue: " << cosValue << std::endl;
    }
    return resultQue;
}

std::string WebPageQuery::createRetJsonStr(std::priority_queue<CandidateDoc> &resultQue, const std::vector<std::string> &queryWords)
{
    std::cout << "匹配到的文章个数：" << resultQue.size() << std::endl;

    Json::Value retJson;
    retJson["code"] = 0;

    for (auto &keyword : queryWords)
    {
        retJson["keywords"].append(keyword);
    }
    int max_index = resultQue.size() > 5 ? 5 : resultQue.size();
    for (int i = 0; i < max_index; ++i)
    {
        CandidateDoc canDoc = resultQue.top();
        Json::Value value;
        value["docId"] = canDoc._docId;
        value["title"] = canDoc._docTitle;
        value["url"] = canDoc._docUrl;
        value["desc"] = canDoc._docDesc;

        retJson["data"].append(value);
        resultQue.pop();
    }
    Json::FastWriter fastWriter;
    std::string retJsonStr = fastWriter.write(retJson);
    LogInfo("匹配到结果：%s", retJsonStr.c_str());
    return retJsonStr;
}

std::string WebPageQuery::returnNoAnswer()
{
    Json::Value retJson;
    retJson["code"] = 1;
    retJson["data"] = "未查询到结果";
    LogInfo("没有匹配到结果");
    Json::FastWriter fastWriter;
    std::string retJsonStr = fastWriter.write(retJson);
    return retJsonStr;
}

void WebPageQuery::loadLibray()
{
    // 加载网页偏移库
    std::ifstream ifsOffset(ConfigMgr::Inst()["Web"]["newoffset"]);
    if (!ifsOffset.good())
    {
        LogError("open %s error", ConfigMgr::Inst()["Web"]["newoffset"].c_str());
        exit(-1);
    }
    std::string line;
    while (getline(ifsOffset, line))
    {
        std::istringstream iss(line);
        int docId, startIndex, length;
        iss >> docId >> startIndex >> length;
        _offsetLib.insert(std::make_pair(docId, std::make_pair(startIndex, length)));
    }
    ifsOffset.close();
    LogInfo("加载网页偏移库成功，大小 %d：", _offsetLib.size());

    // 加载倒排索引库
    std::ifstream ifsInvert(ConfigMgr::Inst()["Web"]["invertindex"]);
    if (!ifsInvert)
    {
        LogError("open %s error", ConfigMgr::Inst()["Web"]["invertindex"].c_str());
        exit(-1);
    }
    std::string word;
    int docId;
    double weight;
    while (getline(ifsInvert, line))
    {
        std::istringstream iss(line);
        iss >> word;
        while (iss >> docId)
        {
            iss >> weight;
            _invertIndexTable[word].insert(std::make_pair(docId, weight));
            // std::cout << " docId " << docId << " weight: " << weight;
        }
    }
    ifsInvert.close();
    LogInfo("加载倒排索引库成功，大小 : %d", _invertIndexTable.size());
}