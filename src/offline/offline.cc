#include "LogMgr.h"
#include "ConfigMgr.h"

int main()
{
    LogDebug("Hello, world!");
    ConfigMgr::GetInstance();

    return 0;
}
