#include "LRUCache.h"
#include "MutexLock.h"

#include <iostream>
#include <fstream>
#include <sstream>

LRUCache::LRUCache(int num)
    : _capacity(num)
{
}

LRUCache::LRUCache(const LRUCache &cache)
{
    _hashMap = cache._hashMap;
    _resultList = cache._resultList;
    _pendingUpdateList = cache._pendingUpdateList;
    _capacity = cache._capacity;
}

// 往缓存中添加数据
void LRUCache::addElement(const std::string &key, const std::string &value)
{
    // 1. 如果要添加的数据已存在
    if (_hashMap.count(key))
    {
        // 将该数据移到链表头部
        _resultList.splice(_resultList.begin(), _resultList, _hashMap[key]);
        // 更改哈希表中key指向的迭代器
        _hashMap[key] = _resultList.begin();
    }
    // 2. 如果要添加的数据不存在
    else
    {
        // 如果缓存已满
        if (static_cast<int>(_hashMap.size()) == _capacity)
        {
            // 删除链表尾部元素
            _hashMap.erase(_resultList.back().first);
            _resultList.pop_back();
        }
        // 添加新数据到链表头部
        _resultList.push_front(std::make_pair(key, value));
        _hashMap.insert(std::make_pair(key, _resultList.begin()));
    }
    // 添加到待更新的节点List
    _pendingUpdateList.push_back(std::make_pair(key, value));
}
bool LRUCache::getElement(const std::string &key, std::string &value)
{
    if (_hashMap.count(key))
    {
        _resultList.splice(_resultList.begin(), _resultList, _hashMap[key]);
        value = _hashMap[key]->second;
        return true;
    }
    return false;
}

// 从文件中读取缓存信息
void LRUCache::readFromFile(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs.good())
    {
        std::cerr << "open file " << filename << " failed!" << std::endl;
        return;
    }

    std::string command, key, value, line;
    while (getline(ifs, line))
    {
        std::istringstream iss(line);
        iss >> command >> key >> value;
        key = command + " " + key;
        addElement(key, value);
    }
    ifs.close();
}

// 将缓存信息写入文件
void LRUCache::writeToFile(const std::string &filename)
{
    std::ofstream ofs(filename);
    if (!ofs.good())
    {
        std::cerr << "open file " << filename << " failed!" << std::endl;
        return;
    }

    for (auto &pair : _resultList)
    {
        ofs << pair.first << " " << pair.second << "\n";
    }
    ofs.close();
}

// 更新缓存信息
void LRUCache::update(const LRUCache &rhs)
{
    MutexLock lock;
    for (auto &pair : rhs.getPendingUpdateList())
    {
        addElement(pair.first, pair.second);
    }
}

// 获取待更新的节点List
std::list<std::pair<std::string, std::string>> LRUCache::getPendingUpdateList() const
{
    MutexLock lock;
    return _pendingUpdateList;
}

void LRUCache::clearPendingUpdateList()
{
    _pendingUpdateList.clear();
}

void LRUCache::getCache() const
{
    std::cout << "Cache Info: " << std::endl;
    for (auto &pair : _resultList)
    {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
}
