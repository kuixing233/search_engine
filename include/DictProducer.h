#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

class DictProducer
{
public:
    DictProducer();
    ~DictProducer();
    void buildDict(); // 创建词典
    void buildIndex(); // 创建词典索引
    void storeOnDisk();

private:
    std::size_t getByteNum_UTF8(const char c);
    bool isChineseCharacter(const std::string &utf8Str);

private:
    std::vector<std::pair<std::string, int>> _dict; // 词典
    std::map<std::string, std::set<unsigned long>> _index; // 词典索引

};

#endif
