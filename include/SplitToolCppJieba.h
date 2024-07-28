#ifndef __KX_SPLITTOOLCPPJIEBA_H__
#define __KX_SPLITTOOLCPPJIEBA_H__

#include "SplitTool.h"
#include "Singleton.h"
#include "simhash/cppjieba/Jieba.hpp"


class SplitToolCppJieba 
: public SplitTool
, public Singleton<SplitToolCppJieba>
{
public:
    friend class Singleton<SplitToolCppJieba>;

    ~SplitToolCppJieba() override;
    std::vector<std::string> cut(const std::string& str) override;

private:
    SplitToolCppJieba();
    cppjieba::Jieba _jieba;
};

#endif