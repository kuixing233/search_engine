#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__

#include "WebPage.h"
#include "Singleton.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

class CandidateDoc
{
    friend bool operator < (const CandidateDoc& lhs, const CandidateDoc& rhs);
public:
    CandidateDoc(int docId, const std::string& title, const std::string& url, const std::string& desc, double cosValue)
        : _docId(docId)
        , _docTitle(title)
        , _docUrl(url)
        , _docDesc(desc)
        , _cosValue(cosValue)
    {

    }

public:
    int _docId;
    double _cosValue;
    std::string _docTitle;
    std::string _docUrl;
    std::string _docDesc;
};

class WebPageQuery
    : public Singleton<WebPageQuery>
{
    friend Singleton<WebPageQuery>;
public:
    std::string doQuery(const std::string& str); // 执行查询，返回结果

private:
    void loadLibray();  // 加载库文件
    std::vector<double> getQueryWordsWeightVector(std::unordered_map<std::string, int> &queryWordsFreq);
    bool executeQuery(const std::vector<std::string> & queryWords, std::vector<std::pair<int, std::vector<double>>> & resultVec);
    std::map<int, double> getCosSimilarity(const std::vector<std::pair<int, std::vector<double>>> & resultVec, const std::vector<double>& base);
    std::priority_queue<CandidateDoc> getResultQue(const std::map<int, double> &cosSimilarity, const std::vector<std::string> &queryWords);
    std::string createRetJsonStr(std::priority_queue<CandidateDoc> &resultQue, const std::vector<std::string> &queryWords);
    std::string returnNoAnswer();

private:
    WebPageQuery();
    // std::unordered_map<int, WebPage> _pageLib; // 网页库
    std::unordered_map<int, std::pair<int, int>> _offsetLib; // 网页偏移库
    std::unordered_map<std::string, std::set<std::pair<int, double>>> _invertIndexTable; // 倒排索引库
};

#endif