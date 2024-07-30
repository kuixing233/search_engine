#include "ConfigMgr.h"
#include <fstream>
#include <sstream>
#include <iostream>

ConfigMgr::ConfigMgr()
{
    std::string config_path = "../conf/config.ini";
    std::ifstream config_file(config_path);
    if (!config_file.is_open())
    {
        std::cerr << "无法打开配置文件: " << config_path << std::endl;
        return;
    }

    std::string line;
    std::string current_section;
    while (std::getline(config_file, line))
    {
        // 去除行首尾的空白字符
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        // 跳过注释和空行
        if (line.empty() || line[0] == ';' || line[0] == '#')
        {
            continue;
        }

        // 处理section
        if (line.front() == '[' && line.back() == ']')
        {
            current_section = line.substr(1, line.size() - 2);
            continue;
        }

        // 处理key-value对
        auto delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos)
        {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            // 去除key和value的首尾空白字符
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            _config_map[current_section]._section_datas[key] = value;
        }
    }
}

ConfigMgr::~ConfigMgr()
{
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