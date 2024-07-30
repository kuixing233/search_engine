#ifndef __KX_CACHEMANAGER_H__
#define __KX_CACHEMANAGER_H__

#include "LRUCache.h"
#include "Singleton.h"
#include "TimerFd.h"

#include <vector>

class CacheManager
    : public Singleton<CacheManager>
{
    friend class Singleton<CacheManager>;
public:
    // 从磁盘文件中读取缓存信息
    void initCache(size_t num, const std::string & filename);
    // 获取某个缓存
    LRUCache & getCache(size_t index);
    // 定时更新所有缓存，增量更新策略
    void periodicUpdateCaches();

    // 定时更新所有缓存，读写分离策略
    void periodicUpdateCaches2();
private:
    CacheManager();
    std::vector<LRUCache> _cacheList;
};

#endif