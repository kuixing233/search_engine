# search_engine
🔍基于RSS的全文搜索引擎

## 遇到的问题
### 1. vector在删除元素之后的迭代器失效
```c++
std::vector<std::string> wordList = SplitToolCppJieba::GetInstance()->cut(str);
for (auto it = wordList.begin(); it != wordList.end();)
{
    if (stopWordList.find(*it) != stopWordList.end())
    {
        wordList.erase(it);
    }
    else // 没有删除元素才让迭代器向后移动
    {
        ++it;
    }
}
```