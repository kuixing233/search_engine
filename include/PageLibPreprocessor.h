#ifndef __KX_PAGELIBPREPROCESSOR_H__
#define __KX_PAGELIBPREPROCESSOR_H__

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "WebPage.h"

class PageLibPreprocessor
{
public:
    PageLibPreprocessor();
    void buildPageAndOffsetLib(); // 创建网页库和网页偏移库
    void buildInvertIndexTable(); // 创建倒排索引表
    void storeOnDisk();           // 将经过预处理之后的网页库、网页偏移库和倒排索引表写回磁盘上

private:
    std::set<WebPage> _pageLib;                                                          // 网页库容器对象
    std::unordered_map<int, std::pair<int, int>> _offsetLib;                             // 网页偏移库对象
    std::unordered_map<std::string, std::set<std::pair<int, double>>> _invertIndexTable; // 倒排索引表对象
};

#endif