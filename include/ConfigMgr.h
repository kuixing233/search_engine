#ifndef __KX_CONFIGMGR_H__
#define __KX_CONFIGMGR_H__

#include <map>
#include <string>

struct SectionInfo
{
    SectionInfo() {}
    ~SectionInfo()
    {
        _section_datas.clear();
    }

    SectionInfo(const SectionInfo &src)
    {
        _section_datas = src._section_datas;
    }

    SectionInfo &operator=(const SectionInfo &src)
    {
        if (&src == this)
        {
            return *this;
        }

        this->_section_datas = src._section_datas;
        return *this;
    }

    std::string operator[](const std::string &key)
    {
        if (_section_datas.find(key) == _section_datas.end())
        {
            return "";
        }
        return _section_datas[key];
    }

    std::map<std::string, std::string> _section_datas;
};

/**
 *  配置读取类，使用单例
 */
class ConfigMgr
{
public:
    static ConfigMgr &Inst()
    {
        static ConfigMgr cfg_mgr;
        return cfg_mgr;
    }
    
    ~ConfigMgr();
    SectionInfo operator[](const std::string &section);

private:
    ConfigMgr();
    ConfigMgr(const ConfigMgr &src) = delete;
    ConfigMgr &operator=(const ConfigMgr &src) = delete;

    // 存储section和key-value对的map
    std::map<std::string, SectionInfo> _config_map;
};

#endif // __KX_CONFIGMGR_H__