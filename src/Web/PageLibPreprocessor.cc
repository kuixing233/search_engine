#include "PageLibPreprocessor.h"
#include "RssParse.h"
#include "DirScanner.h"
#include "ConfigMgr.h"
#include "LogMgr.h"

#include <cmath>

PageLibPreprocessor::PageLibPreprocessor()
{
}

void PageLibPreprocessor::buildPageAndOffsetLib()
{
    DirScanner dirScan;
    dirScan(ConfigMgr::Inst()["Web"]["xml"]);
    const std::vector<std::string> &xmlFiles = dirScan.files();

    int docId = 0;
    int offset = 0;
    int filter = 0;
    for (const auto &xmlFile : xmlFiles)
    {
        std::vector<RssItem> items = RssParse::GetInstance()->parseDoc(xmlFile);
        for (auto item : items)
        {
            if ((item._content.size() + item._desc.size()) == 0)
            {
                // std::cout << "网页：[" << xmlFile << "] 文章[" << item._title << "] 无内容: " << "filter ++ " << filter << std::endl;
                filter++;
                continue;
            }
            item._docId = docId;
            WebPage page(item);
            // 加入网页库
            auto bIt = _pageLib.insert(page);
            if (!bIt.second)
            {
                // std::cout << "网页：[" << xmlFile << "] 文章 [" << item._title << "] 插入网页库失败: " << "filter ++ " << filter << std::endl;
                // 网页去重，两篇文章相似度过高会插入失败
                filter++;
                continue;
            }
            // 加入网页偏移库
            _offsetLib[page.getDocId()] = std::make_pair(offset, page.getDocStr().size());

            ++docId;
            offset += page.getDocStr().size();
        }
    }

    std::cout << "总的文章数：" << docId + filter << std::endl;
    std::cout << "过滤的文章数：" << filter << std::endl;
    std::cout << "有效的文章数：" << docId << std::endl;
}

void PageLibPreprocessor::buildInvertIndexTable()
{
    std::unordered_map<std::string, int> DFMap;
    std::unordered_map<std::string, double> WMap; // 计算并暂存每个词语的权重系数,用于后续的归一化处理

    // 统计每个词语出现的文章个数
    for (auto page : _pageLib)
    {
        for (auto &pair : page.getWordsFreq())
        {
            std::string word = pair.first;
            DFMap[word]++;
        }
    }
    // 文章总数
    double N = _offsetLib.size();
    for (auto page : _pageLib)
    {
        double wNum = 0; // 用于计算归一化因子
        for (auto &pair : page.getWordsFreq())
        {
            std::string word = pair.first;
            int TF = pair.second;   // 某个词在当前文章中出现的次数
            int DF = DFMap[word];   // 包含某个词字的文章个数
            double IDF = log2(N / (DF + 1)); // 逆文档频率
            double w = TF * IDF;    // 词语的权重
            WMap[word] = w; 
            wNum += pow(w, 2);
        }
        // 对每个词的权重进行归一化处理
        for (auto &pair : page.getWordsFreq())
        {
            std::string word = pair.first;
            double w_ = WMap[word] / sqrt(wNum);
            _invertIndexTable[word].insert(std::make_pair(page.getDocId(), w_));
        }
    }
}

void PageLibPreprocessor::storeOnDisk()
{
    // 将网页库写入磁盘
    ofstream ofs(ConfigMgr::Inst()["Web"]["newripepage"]);
    if (!ofs.good())
    {
        LogError("open " + ConfigMgr::Inst()["Web"]["newripepage"] + " fail!");
        exit(-1);        
    }
    for (auto page : _pageLib)
    {
        ofs << page.getDocStr();
    }
    ofs.close();
    LogInfo("网页库保存完成，大小: %u", _pageLib.size());

    // 将网页偏移库写入磁盘
    ofs.open(ConfigMgr::Inst()["Web"]["newoffset"]);
    if (!ofs.good())
    {
        LogError("open " + ConfigMgr::Inst()["Web"]["newoffset"] + " fail!");
        exit(-1);        
    }
    for (const auto &offset : _offsetLib)
    {
        ofs << offset.first << " " << offset.second.first << " " << offset.second.second << "\n";
    }
    ofs.close();
    LogInfo("网页偏移库保存完成，大小: %u", _offsetLib.size());


    // 将倒排索引库写入磁盘
    ofs.open(ConfigMgr::Inst()["Web"]["invertindex"]);
    if (!ofs)
    {
        LogError("open " + ConfigMgr::Inst()["Web"]["invertindex"] + " fail!");
        exit(-1);        
    }
    for (const auto &invertIndex : _invertIndexTable)
    {
        ofs << invertIndex.first;
        for (auto pair : invertIndex.second)
        {
            ofs << " " << pair.first << " " << pair.second;
        }
        ofs << "\n";
    }
    ofs.close();
    LogInfo("倒排索引库保存完成，大小: %u", _invertIndexTable.size());

}