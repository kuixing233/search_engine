#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__

#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <map>

#include "RssParse.h"
#include "MySimHash.h"

const static int TOPK_NUMBER = 20;

class WebPage
{
    friend class PageLibPreProcessor;
    friend bool operator==(const WebPage &lhs, const WebPage &rhs); // 判断两片文档是否相等
    friend bool operator<(const WebPage &lhs, const WebPage &rhs);  // 对文档按_docId进行排序

public:
    WebPage(RssItem &item);

    int getDocId();
    std::string getDoc();
    std::unordered_map<std::string, int> &getWordsFreq();
    std::string getDocStr();

private:
    void doProcessDoc();

private:
    uint64_t _simHashValue;
    std::string _doc;                                // 整篇文档，包括xml在内
    int _docId;                                      // 文档id
    std::string _docTitle;                           // 文档标题
    std::string _docUrl;                             // 文档url
    std::string _docContent;                         // 文档内容
    std::string _docStr;                             // 文章输出的格式化字符串
    std::vector<std::string> _topWords;              // 词频最高的前20个词
    std::unordered_map<std::string, int> _wordsFreq; // 保存每篇文档的所有词语和词频，不包括停用词
};

#endif