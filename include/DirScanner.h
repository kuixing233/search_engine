#ifndef __KX_DIRSCANNER_H__
#define __KX_DIRSCANNER_H__

#include <vector>
#include <string>

class DirScanner
{
public:
    void operator() (const std::string &dirname);
    const std::vector<std::string> &files() const;

    void traverse(const std::string &dirname);      // 得到dirname文件夹下的所有文件

private:
    std::vector<std::string> _files;    
};

#endif