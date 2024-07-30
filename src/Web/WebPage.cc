#include "WebPage.h"
#include <sstream>
#include "MySimHash.h"

WebPage::WebPage(RssItem & item)
    : _docId(item._docId)
    , _docTitle(item._title)
    , _docUrl(item._url)
    , _docContent(item._desc + item._content)
{
    doProcessDoc();
}

void WebPage::doProcessDoc()
{
    std::ostringstream ssm;
    ssm << "<doc>\n\t<docid>" << _docId
        << "</docid>\n\t<title>" << _docTitle 
        << "</title>\n\t<url>" << _docUrl
        << "</url>\n\t<content>" << _docContent
        << "</content>\n</doc>\n";
    _docStr = ssm.str();

    auto myhs = MySimHash::GetInstance();
    _simHashValue = myhs->getSimHashValue(_docContent, TOPK_NUMBER);
    _wordsFreq = myhs->getWordFrec(_docContent);
    for (auto & pair : myhs->getTopK(_docContent, TOPK_NUMBER)) {
        _topWords.push_back(pair.first);
        // std::cout << "topword: " << pair.first << std::endl;
    }
}

int WebPage::getDocId()
{
    return _docId;
}

std::string WebPage::getDoc()
{
    return _doc;
}

std::unordered_map<std::string, int> & WebPage::getWordsFreq()
{
    return _wordsFreq;
}

std::string WebPage::getDocStr()
{
    return _docStr;
}

bool operator==(const WebPage & lhs, const WebPage & rhs)
{
    // 两个文档的指纹的海明距离小于5，认为两个文档相等
    return simhash::Simhasher::isEqual(lhs._simHashValue, rhs._simHashValue, 5);
}

bool operator<(const WebPage & lhs, const WebPage & rhs)
{
    if (lhs == rhs) {
        return false;
    }
    return lhs._docId < rhs._docId;
}
