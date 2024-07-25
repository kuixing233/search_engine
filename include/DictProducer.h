#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class DictProducer
{
public:
    DictProducer();
    ~DictProducer();
    void buildEnDict(); // 创建英文词典
    void buildZhDict(); // 创建中文词典
    void store();

private:
    std::size_t getByteNum_UTF8(const char c);
    bool isChineseCharacter(const std::string &utf8Str);


private:
    std::vector<std::string> _zh_files;                                           // 中文语料库文件的绝对路径集合
    std::vector<std::string> _en_files;                                           // 英文语料库文件的绝对路径集合
    std::unordered_map<std::string, int> _zh_dict;                                // 中文词典，vector比map快，可以用下标
    std::unordered_map<std::string, int> _en_dict;                                // 英文词典
    std::unordered_map<std::string, std::unordered_set<unsigned long>> _zh_index; // 中文词典索引，unordered_map比map查询快，O(1)
    std::unordered_map<std::string, std::unordered_set<unsigned long>> _en_index; // 英文词典索引
};

#endif
