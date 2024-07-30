#ifndef __KX_LRUCACHE_H__
#define __KX_LRUCACHE_H__

#include <unordered_map>
#include <list>

class LRUCache
{
public:
    LRUCache(int num = 10);
    LRUCache(const LRUCache &cache);

    // 往缓存中添加数据
    void addElement(const std::string &key, const std::string &value);
    // 从缓存中获取数据
    bool getElement(const std::string &key, std::string &value);
    // 从文件中读取缓存信息
    void readFromFile(const std::string &filename);
    // 将缓存信息写入文件
    void writeToFile(const std::string &filename);
    // 更新缓存信息
    void update(const LRUCache &rhs);
    // 获取待更新的节点List
    std::list<std::pair<std::string, std::string>> getPendingUpdateList() const;
    // 清空待更新的节点List
    void clearPendingUpdateList();

    void getCache() const;

private:
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> _hashMap; // 采用hashTable进行查找
    std::list<std::pair<std::string, std::string>> _resultList;                                         // 保存键值对
    std::list<std::pair<std::string, std::string>> _pendingUpdateList;                                  // 等待更新的节点信息
    int _capacity;                                                                                      // 缓存节点的容量
};

#endif