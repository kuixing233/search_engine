#include "CacheManager.h"
#include "ConfigMgr.h"

// 从磁盘文件中读取缓存信息
void CacheManager::initCache(size_t num, const std::string &filename)
{
    _cacheList.reserve(num);

    for (size_t i = 0; i < num; ++i)
    {
        _cacheList.push_back(LRUCache(10));
        _cacheList[i].readFromFile(filename);
    }


}

// 获取某个缓存
LRUCache & CacheManager::getCache(size_t index)
{
    return _cacheList[index];
}

// 定时更新所有缓存
void CacheManager::periodicUpdateCaches()
{
    std::cout << "定时同步所有缓存" << std::endl;
    
    for (size_t idx = 1; idx < _cacheList.size(); ++idx)
    {
        _cacheList[0].update(_cacheList[idx]);
        _cacheList[idx].clearPendingUpdateList();
    }

    _cacheList[0].writeToFile(ConfigMgr::Inst()["System"]["CACHE_PATH"]);

    for (size_t idx = 1; idx < _cacheList.size(); ++idx)
    {
        _cacheList[idx].update(_cacheList[0]);
        _cacheList[idx].clearPendingUpdateList();
    }
    _cacheList[0].clearPendingUpdateList();
    std::cout << "缓存同步完成" << std::endl;
}

CacheManager::CacheManager()
{
}