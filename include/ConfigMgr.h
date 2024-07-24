#ifndef __KX_CONFIGMGR_H__
#define __KX_CONFIGMGR_H__
/**
 * 类SectionInfo：一个对象表示一个配置文件的键值对，重载了[]，可像访问map一样使用
 * 类ConfigMgr：实际读取配置文件的类，将配置文件的内容按照std::map<std::string, SectionInfo>保存起来
 */

#include "Singleton.h"

// boost库中读取ini配置文件的头文件
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <map>

struct SectionInfo {
	SectionInfo() {}
	~SectionInfo() {
		_section_datas.clear();
	}

	SectionInfo(const SectionInfo& src) {
		_section_datas = src._section_datas;
	}

	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this) {
			return *this;
		}

		this->_section_datas = src._section_datas;
		return *this;
	}

	std::string  operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		// 这里可以添加一些边界检查  
		return _section_datas[key];
	}

	std::map<std::string, std::string> _section_datas;
};

/**
 *  配置读取类，使用单例
 */
class ConfigMgr
    : public Singleton<ConfigMgr>
{
    friend class Singleton<ConfigMgr>;
public:
	~ConfigMgr();

	SectionInfo operator[](const std::string& section);

private:
	ConfigMgr();
	// 存储section和key-value对的map  
	std::map<std::string, SectionInfo> _config_map;
};

#endif