#include "RssParse.h"
#include "LogMgr.h"

RssParse::RssParse()
{
}

// 去除多余标签
void RssParse::rm_html_flag(string &text)
{
    std::regex reg1("<[^>]*>|&nbsp;|　　");
    text = regex_replace(text, reg1, "");
}

vector<RssItem> RssParse::parseDoc(const string &filename)
{
    vector<RssItem> rssVec;
    XMLDocument doc;
    int ret = doc.LoadFile(filename.c_str());
    if (ret != XML_SUCCESS)
    {
        LogError("loadXMLFile %s error", filename.c_str());
        return rssVec;
    }
    XMLElement *root = doc.RootElement();            // <rss>
    XMLElement *channel = root->FirstChildElement(); // <channel>

    // 遍历文件找到所有item，将每个item中的内容存入vector容器
    for (XMLElement *item = channel->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement())
    {
        // 遍历item节点的子节点
        RssItem itemNode;
        for (XMLElement *subElem = item->FirstChildElement(); subElem != NULL; subElem = subElem->NextSiblingElement())
        {
            // title节点，<title>
            if (strcmp(subElem->Name(), "title") == 0)
            {
                string tmp = subElem->GetText();
                // 去除html标签
                rm_html_flag(tmp);
                itemNode._title = tmp;
            }
            // link节点
            else if (strcmp(subElem->Name(), "link") == 0)
            {
                string tmp = subElem->GetText();
                rm_html_flag(tmp);
                itemNode._url = tmp;
            }
            // description
            else if (strcmp(subElem->Name(), "description") == 0)
            {
                string tmp = subElem->GetText();
                rm_html_flag(tmp);
                itemNode._desc = tmp;
            }
            // content
            else if (strcmp(subElem->Name(), "content:encoded") == 0)
            {
                string tmp = subElem->GetText();
                rm_html_flag(tmp);
                itemNode._content = tmp;
            }
        }
        rssVec.push_back(itemNode);
    }
    return rssVec;
}

vector<RssItem> RssParse::parseStr(const string &str, const std::vector<std::string> &queryWords)
{
    vector<RssItem> rssVec;
    XMLDocument doc;
    int ret = doc.Parse(str.c_str());
    if (ret != XML_SUCCESS)
    {
        LogError("loadXmlStr %s error", str.c_str());

        return rssVec;
    }

    XMLElement *rootElement = doc.FirstChildElement("doc");
    if (rootElement)
    {
        string docId = rootElement->FirstChildElement("docid")->GetText();
        string title = rootElement->FirstChildElement("title")->GetText();
        string url = rootElement->FirstChildElement("url")->GetText();
        string content = rootElement->FirstChildElement("content")->GetText();
        string desc = geneDescFromContent(content, queryWords);

        RssItem item;
        item._docId = atoi(docId.c_str());
        item._title = title;
        item._url = url;
        item._content = content;
        item._desc = desc;
        rssVec.push_back(item);
    }

    return rssVec;
}

size_t getByteNum_UTF8(const char ch)
{
    if (ch & (1 << 7))
    {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx)
        {
            if (ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

std::string RssParse::geneDescFromContent(const std::string &content, const std::vector<std::string> &queryWords)
{

    // std::vector<std::string> sentences;
    std::map<std::string, int> sentences;
    std::string sentence;
    for (size_t i = 0, cnt = 0; i < content.size(); cnt++)
    {
        int len = getByteNum_UTF8(content[i]);
        std::string word = content.substr(i, len);
        sentence += word;
        i += len;
        if (word == "。" || word == "？" || word == "！")
        {
            for (const auto &word : queryWords)
            {
                if (sentence.find(word) != std::string::npos)
                {
                    sentences[sentence]++;
                }
            }
            // std::cout << "sentence: " << sentence << std::endl;
            sentence.clear();
        }
    }
    if (!sentence.empty())
    {
        for (const auto &word : queryWords)
        {
            if (sentence.find(word) != std::string::npos)
            {
                sentences[sentence]++;
            }
        }
    }

    std::vector<std::pair<std::string, int>> svec(sentences.begin(), sentences.end());
    std::sort(svec.begin(), svec.end(), [](const std::pair<std::string, int> &lhs, const std::pair<std::string, int> &rhs) {
        return lhs.second > rhs.second;
    });

    std::string desc;
    int cnt = 0;
    for (const auto &sentence : svec)
    {
        desc += sentence.first;
        if (cnt ++ >= 3)
        {
            break;
        }
    }
    return desc;
}