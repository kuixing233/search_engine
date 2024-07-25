#include "DictProducer.h"
#include "DirScanner.h"
#include "StopWords.h"
#include "ConfigMgr.h"
#include "LogMgr.h"
#include "SplitToolCppJieba.h"

#include <fstream>
#include <sstream>

DictProducer::DictProducer()
{
    DirScanner dirScanner;
    dirScanner(ConfigMgr::Inst()["KeyRec"]["en_art"]);
    _en_files = dirScanner.files();
    dirScanner(ConfigMgr::Inst()["KeyRec"]["zh_art"]);
    _zh_files = dirScanner.files();
}

void DictProducer::buildEnDict()
{
    const auto &stopWordsList = StopWords::GetInstance()->getStopWordList();

    for (const auto &file : _en_files)
    {
        std::ifstream ifs(file);
        if (!ifs.good())
        {
            LogError("open %s error!", file.c_str());
        }

        std::string line;
        while (getline(ifs, line))
        {
            std::string word;
            std::istringstream iss(line);
            while (iss >> word)
            {
                // 单词中只能出现英文字符，并转为小写
                std::string newWord;
                for (auto c : word)
                {
                    if (isalpha(c))
                    {
                        c = std::tolower(c);
                        newWord += c;
                    }
                }
                if (newWord.empty())
                {
                    continue;
                }
                // 只保存不在停用词列表中的单词
                auto it = stopWordsList.find(newWord);
                if (it != stopWordsList.end())
                {
                    continue;
                }
                ++_en_dict[newWord];
                std::cout << newWord << " : " << _en_dict[newWord] << std::endl;
            }
        }
        ifs.close();
    }

    // 创建英文词典索引
    unsigned long lineNum = 1;
    for (auto it = _en_dict.begin(); it != _en_dict.end(); ++it)
    {
        std::string word = it->first;
        for (const char &c : word)
        {
            std::string cAlpha{c};
            _en_index[cAlpha].insert(lineNum);
        }
        ++lineNum;
    }
}

void DictProducer::buildZhDict()
{
    const auto &stopWordsList = StopWords::GetInstance()->getStopWordList();
    std::string txt; // 中文语料集合字符串
    std::string line;
    for (const auto &file : _zh_files)
    {
        std::ifstream ifs(file);
        if (!ifs.good())
        {
            LogError("open %s error!", file.c_str());
        }
        while (getline(ifs, line))
        {
            txt += line;
        }
        ifs.close();
    }
    // 分词
    const std::vector<std::string> &words = SplitToolCppJieba::GetInstance()->cut(txt);

    // 对得到的每个中文词进行处理
    for (const auto &word : words)
    {
        std::cout << "word: " << word << " size: " << word.size() << " substr: ";
        std::string newWord;
        for (size_t i = 0; i < word.size();)
        {
            int byteNum = getByteNum_UTF8(word[i]);
            std::string substr = word.substr(i, byteNum);
            std::cout << substr << " ";
            i += byteNum;
            if (isChineseCharacter(substr))
            {
                newWord += substr;
            }
        }
        if (newWord.size() <= 3) {
            continue;
        }
        // 只保存不在停用词列表中的单词
        auto it = stopWordsList.find(newWord);
        if (it != stopWordsList.end())
        {
            continue;
        }
        ++_zh_dict[newWord];
        std::cout << "\nnewWord: " << newWord << " : " << _zh_dict[newWord] << std::endl;
    }

    // 创建中文词典索引
    unsigned long lineNum = 1;
    for (auto it = _zh_dict.begin(); it != _zh_dict.end(); ++it)
    {
        std::istringstream iss(it->first);
        std::string word;
        iss >> word;
        for (int i = 0; i < word.size();)
        {
            int byteNum = getByteNum_UTF8(word[0]);
            std::string substr = word.substr(i, byteNum);
            _zh_index[substr].insert(lineNum);
            std::cout << substr << " : " << lineNum << std::endl;
            i += byteNum;
        }
        ++lineNum;
    }
}

void DictProducer::store()
{
    LogInfo("en dict size: %lu, zh dict size: %lu", _en_dict.size(), _zh_dict.size());
    LogInfo("en index size: %lu, zh index size: %lu", _en_index.size(), _zh_index.size());

    std::ofstream ofsEnDict(ConfigMgr::Inst()["KeyRec"]["en_dict"]);
    for (const auto &word : _en_dict)
    {
        ofsEnDict << word.first << " " << word.second << std::endl;
    }
    ofsEnDict.close();
    LogInfo("en dict stored");
    std::ofstream ofsZhDict(ConfigMgr::Inst()["KeyRec"]["zh_dict"]);
    for (const auto &word : _zh_dict)
    {
        ofsZhDict << word.first << " " << word.second << std::endl;
    }
    ofsZhDict.close();
    LogInfo("zh dict stored");

    std::ofstream ofsEnIndex(ConfigMgr::Inst()["KeyRec"]["en_index"]);
    for (const auto &word : _en_index)
    {
        ofsEnIndex << word.first << " ";
        for (const auto &num : word.second)
        {
            ofsEnIndex << num << " ";
        }
        ofsEnIndex << std::endl;
    }
    ofsEnIndex.close();
    LogInfo("en index stored");

    std::ofstream ofsZhIndex(ConfigMgr::Inst()["KeyRec"]["zh_index"]);
    for (const auto &word : _zh_index)
    {
        ofsZhIndex << word.first << " ";
        for (const auto &num : word.second)
        {
            ofsZhIndex << num << " ";
        }
        ofsZhIndex << std::endl;
    }
    ofsZhIndex.close();
    LogInfo("zh index stored");
}

std::size_t DictProducer::getByteNum_UTF8(const char c)
{
    if ((c & 0x80) == 0)
        return 1; // 1字节字符
    if ((c & 0xE0) == 0xC0)
        return 2; // 2字节字符
    if ((c & 0xF0) == 0xE0)
        return 3; // 3字节字符
    if ((c & 0xF8) == 0xF0)
        return 4; // 4字节字符

    return 0; // 不是有效的UTF-8字符
}

bool DictProducer::isChineseCharacter(const std::string &utf8Str)
{
    if (utf8Str.empty())
        return false;

    // 获取字符的字节数
    unsigned char c = utf8Str[0];
    int byteCount = getByteNum_UTF8(c);
    if (byteCount == 0 || utf8Str.size() != byteCount)
        return false;

    // 计算Unicode码点
    uint32_t unicode = 0;
    for (int i = 0; i < byteCount; ++i)
    {
        unicode <<= 6;
        unicode |= (utf8Str[i] & 0x3F);
    }

    unicode &= ~(1 << (6 * byteCount - 1)); // 清除多余位

    // 判断Unicode是否在中文字符范围内
    return (unicode >= 0x4E00 && unicode <= 0x9FA5);
}

DictProducer::~DictProducer()
{
}
