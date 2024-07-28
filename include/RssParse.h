#ifndef __KX_RSSPARSE_h__
#define __KX_RSSPARSE_h__

#include "Singleton.h"

#include "tinyxml2.h"

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <fstream>

using std::cout;
using std::endl;
using std::ofstream;
using std::string;
using std::vector;

using namespace tinyxml2;

struct RssItem
{
    int _docId;
    string _title;
    string _url;
    string _desc;
    string _content;
};

class RssParse
    : public Singleton<RssParse>
{
    friend class Singleton<RssParse>;
public:
    vector<RssItem> parseDoc(const string &filename);
    vector<RssItem> parseStr(const string &str, const std::vector<std::string> &queryWords);


private:
    RssParse();
    void rm_html_flag(string &text);
    std::string geneDescFromContent(const std::string& content, const std::vector<std::string>& queryWords);
};

#endif