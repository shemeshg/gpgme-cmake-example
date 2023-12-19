#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"
#include "WatchWaitAndNoneWaitRunCmd.h"

std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmd>  getInterfaceWatchWaitAndNoneWaitRunCmd()
{
    auto p = std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmd>(new WatchWaitAndNoneWaitRunCmd);
    
    return p;
}
