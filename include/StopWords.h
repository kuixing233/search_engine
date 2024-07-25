#ifndef __KX_STOPWORDS_H__
#define __KX_STOPWORDS_H__

#include "Singleton.h"

#include <unordered_set>

class StopWords 
    : public Singleton<StopWords>
{
    friend Singleton<StopWords>;
public:
    const std::unordered_set<std::string> &getStopWordList() const;

private:
    StopWords();

    std::unordered_set<std::string> _stop_word_list;
};

#endif