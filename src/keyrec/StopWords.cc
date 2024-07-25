#include "StopWords.h"
#include "ConfigMgr.h"
#include "DirScanner.h"
#include "LogMgr.h"

#include <fstream>

StopWords::StopWords()
{
    DirScanner dirScanner;
    dirScanner(ConfigMgr::Inst()["KeyRec"]["stop_words"]);

    for (const auto &file : dirScanner.files())
    {
        std::ifstream ifs(file);
        if (!ifs.good())
        {
            LogError("open %s error!", file.c_str());
        }

        std::string word;
        while (std::getline(ifs, word))
        {
            _stop_word_list.insert(word);
        }
    }
}

const std::unordered_set<std::string> &StopWords::getStopWordList() const
{
    return _stop_word_list;
}
