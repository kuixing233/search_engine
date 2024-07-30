#ifndef __MYSIMHASH_H__
#define __MYSIMHASH_H__

//this define can avoid some logs which you don't need to care about.
#define LOGGER_LEVEL LL_WARN

#include "simhash/Simhasher.hpp"
#include "Singleton.h"

class MySimHash
    : public Singleton<MySimHash>
{
    friend Singleton<MySimHash>;
public:
    ~MySimHash();
    // 获得文档str的指纹
    uint64_t getSimHashValue(const std::string& str, size_t topN);
    // 获得文档str的topN个最重要的关键词以及它们的权重
    std::vector<std::pair<std::string, double>> getTopK(const std::string& str, size_t topN);
    std::unordered_map<std::string, int> getWordFrec(const std::string& str);

private:
    MySimHash();

    simhash::Simhasher _simHasher;
};

#endif