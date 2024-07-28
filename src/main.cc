#include "SearchEngine.h"
#include "SplitToolCppJieba.h"
#include "MySimHash.h"
#include "DictProducer.h"
#include "PageLibPreprocessor.h"
#include "StopWords.h"
#include "ConfigMgr.h"

void test1()
{
    // 是否重新加载语料库
    if (ConfigMgr::Inst()["System"]["IS_RELOADCORPUS"] == "true")
    {
        DictProducer dp;
        dp.buildDict();
        dp.buildIndex();
        dp.storeOnDisk();
    }

    // 是否重新加载网页库
    if (ConfigMgr::Inst()["System"]["IS_RELOADWEBPAGE"] == "true")
    {
        PageLibPreprocessor plp;
        plp.buildPageAndOffsetLib();
        plp.buildInvertIndexTable();
        plp.storeOnDisk();
    }

    // 初始化
    StopWords::GetInstance();
    SplitToolCppJieba::GetInstance();
    MySimHash::GetInstance();
    KeyRecommander::GetInstance();
    WebPageQuery::GetInstance();

    // 根据配置文件启动搜索引擎
    string ip = ConfigMgr::Inst()["System"]["IP"];
    string port = ConfigMgr::Inst()["System"]["PORT"];
    string threadNum = ConfigMgr::Inst()["System"]["THREADNUM"];
    string queSize = ConfigMgr::Inst()["System"]["QUESIZE"];

    SearchEngine es(atoi(threadNum.c_str()),
                    atoi(queSize.c_str()),
                    ip,
                    atoi(port.c_str()));
    es.start();
}

int main()
{
    test1();
    return 0;
}