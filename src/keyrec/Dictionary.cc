#include "Dictionary.h"
#include "ConfigMgr.h"
#include "LogMgr.h"

#include <fstream>
#include <iostream>
#include <sstream>

Dictionary::Dictionary()
{
    _dict.reserve(65535);
    _index.reserve(4096);

    init();
}

Dictionary::~Dictionary()
{

}

void Dictionary::init()
{
    std::ifstream enDictIfs(ConfigMgr::Inst()["KeyRec"]["en_dict"]);
    if (!enDictIfs.is_open()) {
        LogError("open file %s error!", ConfigMgr::Inst()["KeyRec"]["en_dict"].c_str());
    }
    std::ifstream zhDictIfs(ConfigMgr::Inst()["KeyRec"]["zh_dict"]);
    if (!zhDictIfs.is_open()) {
        LogError("open file %s error!", ConfigMgr::Inst()["KeyRec"]["zh_dict"].c_str());
    }
    std::string line;
    std::string word;
    int frequency;
    while (getline(enDictIfs, line))
    {
        std::istringstream iss(line);
        iss >> word >> frequency;
        _dict.push_back(std::make_pair(word, frequency));
    }
    std::size_t cnDictOffset = _dict.size();
    while (getline(zhDictIfs, line))
    {
        std::istringstream iss(line);
        iss >> word >> frequency;
        _dict.push_back(std::make_pair(word, frequency));
    }
    enDictIfs.close();
    zhDictIfs.close();

    std::cout << "cnDictOffset: " << cnDictOffset << std::endl;

    std::ifstream enIdxIfs(ConfigMgr::Inst()["KeyRec"]["en_index"]);
    if (!enIdxIfs.is_open()) {
        LogError("open file %s error!", ConfigMgr::Inst()["KeyRec"]["en_index"].c_str());

    }
    std::ifstream zhIdxIfs(ConfigMgr::Inst()["KeyRec"]["zh_index"]);
    if (!zhIdxIfs.is_open()) {
        LogError("open file %s error!", ConfigMgr::Inst()["KeyRec"]["zh_index"].c_str());
    }

    int index;
    while (getline(enIdxIfs, line))
    {
        std::istringstream iss(line);
        iss >> word;
        while (iss >> index) {
            _index[word].insert(index);
        }
    }
    while (getline(zhIdxIfs, line))
    {
        std::istringstream iss(line);
        iss >> word;
        while (iss >> index) {
            _index[word].insert(cnDictOffset - 1 + index);
        }
    }
    enIdxIfs.close();
    zhIdxIfs.close();

    LogInfo("total dict size: %d", _dict.size());
    LogInfo("total index size: %d", _index.size());
}

std::vector<std::pair<std::string, int>> & Dictionary::getDict()
{
    return _dict;
}

std::unordered_map<std::string, std::set<int>> & Dictionary::getIndexTable()
{
    return _index;
}

void Dictionary::print(bool dict)
{
    if (dict) {
        for (auto it = _dict.begin(); it != _dict.end(); ++ it) {
            std::cout << it->first << " " << it->second << std::endl;
        }
        std::cout << "total dict size: " << _dict.size() << std::endl;
    }
    else {
        for (auto it = _index.begin(); it != _index.end(); ++ it) {
            std::cout << it->first << " ";
            for (auto itSet = it->second.begin(); itSet != it->second.end(); ++ itSet) {
                std::cout << *itSet << " ";
            }
            std::cout << "\n";
        }
        std::cout << "total index size: " << _index.size() << std::endl;
    }
}
