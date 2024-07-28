#ifndef __KX_SPLITTOOL_H__
#define __KX_SPLITTOOL_H__

#include <vector>
#include <string>

class SplitTool
{
public:
    SplitTool() {

    }
    virtual ~SplitTool() {
        
    }
    
    virtual std::vector<std::string> cut(const std::string &str) = 0;
};

#endif