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
}

void DictProducer::buildDict()
{
    // 获取停用词列表
    const auto &stopWordsList = StopWords::GetInstance()->getStopWordList();

    // 获取英文语料库文件
    DirScanner dirScanner;
    dirScanner(ConfigMgr::Inst()["Key"]["english"]);
    std::vector<std::string> en_files = dirScanner.files();

    std::map<std::string, int> en_dict;                                // 英文词典
    // 遍历每篇英文文档，保证文件格式一致
    for (const auto &file : en_files)
    {
        // 二进制方式读取，保证文件格式一致
        std::ifstream ifs(file, std::ios::binary);
        if (!ifs.good())
        {
            LogError("open %s error!", file.c_str());
            exit(-1);
        }

        // 逐行读取
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
                if (stopWordsList.find(newWord) != stopWordsList.end())
                {
                    continue;
                }
                ++en_dict[newWord];
                // std::cout << newWord << " : " << en_dict[newWord] << std::endl;
            }
        }
        ifs.close();
    }
    for (const auto &elem : en_dict)
    {
        _dict.push_back(elem);
    }

    dirScanner(ConfigMgr::Inst()["Key"]["chinese"]);
    std::vector<std::string> zh_files = dirScanner.files();

    std::map<std::string, int> zh_dict;                                // 中文词典

    for (const auto &file : zh_files)
    {
        std::ifstream ifs(file, std::ios::binary);
        if (!ifs.good())
        {
            LogError("open %s error!", file.c_str());
            exit(-1);
        }
        // 一次性读取整个文件内容
        ifs.seekg(0, std::ios::end);
        std::streampos length = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::string content;
        // 读取文件内容
        content.resize(static_cast<std::size_t>(length));
        ifs.read(&content[0], length);

        // 使用Jieba库分词
        std::vector<std::string> words = SplitToolCppJieba::GetInstance()->cut(content);

        // 对得到的每个中文词进行处理
        for (const auto &word : words)
        {
            // 这个word不是中文，不保存
            if (!isChineseCharacter(word))
            {
                continue;
            }
            // 只保存不在停用词列表中的单词
            if (stopWordsList.find(word) == stopWordsList.end())
            {
                ++zh_dict[word];
                // std::cout << word << " : " << zh_dict[word] << std::endl;
            }
        }
        ifs.close();
    }
    for (const auto &elem : zh_dict)
    {
        _dict.push_back(elem);
    }
}

void DictProducer::buildIndex()
{
    int i = 0;  // 记录下标
    for (auto elem : _dict) {
        string word = elem.first;
        size_t charNums = word.size() / getByteNum_UTF8(word[0]);
        for (size_t idx = 0, n = 0; n != charNums; ++ n) {
            size_t charLen = getByteNum_UTF8(word[idx]);
            string subWord = word.substr(idx, charLen);

            _index[subWord].insert(i);
            idx += charLen;
        }
        ++ i;
    }
}

void DictProducer::storeOnDisk()
{
    std::ofstream ofsDict(ConfigMgr::Inst()["Key"]["dict"]);
    if (!ofsDict.good())
    {
        LogError("open %s error!", ConfigMgr::Inst()["Key"]["dict"].c_str());
        exit(-1);
    }
    for (const auto &elem : _dict)
    {
        ofsDict << elem.first << " " << elem.second << std::endl;
    }
    ofsDict.close();
    LogInfo("词典库保存完成，大小：%d", _dict.size());

    std::ofstream ofsIndex(ConfigMgr::Inst()["Key"]["index"]);
    if (!ofsIndex.good())
    {
        LogError("open %s error!", ConfigMgr::Inst()["Key"]["index"].c_str());
        exit(-1);
    }
    for (const auto &elem : _index)
    {
        ofsIndex << elem.first << " ";
        for (const auto &line : elem.second)
        {
            ofsIndex << line << " ";
        }
        ofsIndex << std::endl;
    }
    ofsIndex.close();
    LogInfo("词典索引库保存完成，大小：%d", _index.size());
}

size_t DictProducer::getByteNum_UTF8(const char byte)
{
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}

std::size_t getByteNum2_UTF8(const char c)
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
    int byteCount = getByteNum2_UTF8(c);
    if (byteCount == 0)
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
