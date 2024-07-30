#include "CacheManager.h"
#include "ConfigMgr.h"

// 从磁盘文件中读取缓存信息
void CacheManager::initCache(size_t num, const std::string &filename)
{
    _cacheList.reserve(2 * num);

    for (size_t i = 0; i < 2 * num; ++i)
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
    std::cout << "定时同步所有缓存。。。" << std::endl;
    
    for (size_t idx = 1; idx < _cacheList.size(); ++idx)
    {
        _cacheList[0].update(_cacheList[idx]);
        // _cacheList[idx].clearPendingUpdateList();
    }

    _cacheList[0].writeToFile(ConfigMgr::Inst()["System"]["CACHE_PATH"]);

    for (size_t idx = 1; idx < _cacheList.size(); ++idx)
    {
        _cacheList[idx].update(_cacheList[0]);
        // _cacheList[idx].clearPendingUpdateList();
    }
    // _cacheList[0].clearPendingUpdateList();

    std::cout << "缓存同步完成" << std::endl;

}

void CacheManager::periodicUpdateCaches2()
{
    std::cout << "定时同步所有缓存。。。" << std::endl;

    // 将读缓存的内容都拷贝到写缓存中，读缓存加锁
    cout << "将读缓存的内容都拷贝到写缓存中" << endl;
    // 读缓存1：3、2、1
    // 读缓存1：4、3、2、1
    for (size_t idx = 1; idx < _cacheList.size() / 2; ++idx)
    {
        // 5个读缓存，5个写缓存
        // 0-4是读缓存，5-9是写缓存
        int i = _cacheList.size() / 2 + idx;
        _cacheList[idx].updateToBack(_cacheList[i]);
        // 写缓存6：4、3、2、1
        cout << "读缓存" << idx << "的内容拷贝到写缓存" << i << "中，写缓存大小: " << _cacheList[i].getResultList().size() << endl;
    }

    // 将写缓存的内容都更新到主缓存中
    cout << "将写缓存的内容都更新到主缓存中" << endl;
    for (size_t idx = _cacheList.size() / 2 + 1; idx < _cacheList.size(); ++idx)
    {
        // 主缓存0：3、2、1
        // 主缓存0：4、3、2、1 --> 3、2、1、4
        _cacheList[0].update(_cacheList[idx]);
    }

    cout << "0缓存大小：" << _cacheList[0].getResultList().size() << endl;
    _cacheList[0].writeToFile(ConfigMgr::Inst()["System"]["CACHE_PATH"]);

    // 将主缓存广播回来的内容都更新到写缓存中
    cout << "将主缓存广播回来的内容都拷贝到写缓存中" << endl;
    for (size_t idx = _cacheList.size() / 2 + 1; idx < _cacheList.size(); ++idx)
    {
        // 10个缓存，加备用20个缓存，0-> 20 / 2 + 0 = 10，1-> 20 / 2 + 1 = 11
        cout << "主缓存的内容更新到写缓存" << idx << "中" << endl;
        // 写缓存：3、2、1、4
        _cacheList[idx].update(_cacheList[0]);
    }

    // 写缓存和读缓存进行swap操作，读缓存加锁
    cout << "写缓存和读缓存进行swap操作" << endl;
    for (size_t idx = 1; idx < _cacheList.size() / 2; ++idx)
    {
        int i = _cacheList.size() / 2 + idx;
        // 读缓存：3、2、1、4
        _cacheList[idx].swapBack(_cacheList[i]);
        _cacheList[i].clearCache();
    }
    std::cout << "缓存同步完成" << std::endl;
}

CacheManager::CacheManager()
{
}