#include "LogMgr.h"
#include "ConfigMgr.h"
#include "DirScanner.h"
#include "SplitToolCppJieba.h"
#include "DictProducer.h"
#include "Dictionary.h"
#include "KeyRecommander.h"

void test1()
{
    LogDebug("Hello, world!");

    DirScanner dirs;
    dirs((ConfigMgr::Inst()["KeyRec"]["stop_words"]));

    for (auto file : dirs.files())
    {
        std::cout << "file: " << file << std::endl;
    }

    SplitTool *jieba = SplitToolCppJieba::GetInstance();
    std::string str = "我爱北京天安门";
    std::vector<std::string> words = jieba->cut(str);
    for (auto word : words)
    {
        std::cout << word << std::endl;
    }
}

void test2()
{
    // DictProducer dict;
    // dict.buildEnDict();
    // dict.buildZhDict();
    // dict.store();

    // Dictionary::GetInstance()->print(false);
    KeyRecommander keyrec("王道");
    keyrec.execute();
}

int main()
{
    test2();
    return 0;
}
