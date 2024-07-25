#ifndef __KX_DICTIONARY_H__
#define __KX_DICTIONARY_H__

#include "Singleton.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class Dictionary
    : public Singleton<Dictionary>
{
    friend Singleton<Dictionary>;
public:
    Dictionary();
    ~Dictionary();

    std::vector<std::pair<std::string, int>> &getDict();             // 获取词典
    std::unordered_map<std::string, std::set<int>> &getIndexTable(); // 获取索引表
    void print(bool dict = true);

private:
    void init(); // 通过词典文件路径初始化词典

    std::vector<std::pair<std::string, int>> _dict;             // 词典
    std::unordered_map<std::string, std::set<int>> _index;      // 索引表
};

#endif
