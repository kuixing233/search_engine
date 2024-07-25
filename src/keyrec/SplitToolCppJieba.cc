#include "SplitToolCppJieba.h"
#include "ConfigMgr.h"

SplitToolCppJieba::SplitToolCppJieba()
    : _jieba(ConfigMgr::Inst()["CppJieba"]["DICT_PATH"],
             ConfigMgr::Inst()["CppJieba"]["HMM_PATH"], 
             "")
            //  ConfigMgr::Inst()["CppJieba"]["USER_DICT_PATH"])
{
}

SplitToolCppJieba::~SplitToolCppJieba()
{

}

const std::vector<std::string> SplitToolCppJieba::cut(const std::string &str)
{
    std::vector<std::string> words;
    _jieba.CutForSearch(str, words);
    return words;
}