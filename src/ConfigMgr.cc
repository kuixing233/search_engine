#include "ConfigMgr.h"

// boost库中读取ini配置文件的头文件
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <iostream>

ConfigMgr::ConfigMgr()
{
	std::cout << "ConfigMgr()" << std::endl;

	std::string config_path = "../conf/config.ini";
	std::cout << "Config path: " << config_path << std::endl;

	// 使用Boost.PropertyTree来读取INI文件 => 跨平台
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path, pt);

	// 遍历INI文件中的所有section
	for (const auto &section_pair : pt)
	{
		const std::string &section_name = section_pair.first;
		const boost::property_tree::ptree &section_tree = section_pair.second;

		// 对于每个section，遍历其所有的key-value对
		std::map<std::string, std::string> section_config;
		for (const auto &key_value_pair : section_tree)
		{
			const std::string &key = key_value_pair.first;
			const std::string &value = key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}
		SectionInfo sectionInfo;
		sectionInfo._section_datas = section_config;
		// 将section的key-value对保存到config_map中
		_config_map[section_name] = sectionInfo;
	}

	// 输出所有的section和key-value对
	std::cout << "-----------------------------------------------------------\n";
	std::cout << "Config path: " << config_path << std::endl;
	for (const auto &section_entry : _config_map)
	{
		const std::string &section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto &key_value_pair : section_config._section_datas)
		{
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
	std::cout << "-----------------------------------------------------------\n";
}

ConfigMgr::~ConfigMgr()
{
	std::cout << "~ConfigMgr()" << std::endl;
	_config_map.clear();
}

SectionInfo ConfigMgr::operator[](const std::string &section)
{
	if (_config_map.find(section) == _config_map.end())
	{
		return SectionInfo();
	}
	return _config_map[section];
}