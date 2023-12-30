#pragma once
#include <memory>
#include <functional>
#include "InterfaceWatchWaitAndNoneWaitRunCmdItem.h"

class InterfaceWatchWaitAndNoneWaitRunCmd {
    public:
    virtual ~InterfaceWatchWaitAndNoneWaitRunCmd(){}
    std::function<void()> callback=[&](){};

    virtual std::vector<std::string> getWaitItemsUids()=0;
    virtual std::vector<std::string> getNoneWaitItemsUids()=0;
    virtual InterfaceWatchWaitAndNoneWaitRunCmdItem *addWithWait(std::string uniqueId, std::string fileName, std::string tmpFolder,std::string  vscodePath) = 0;
    virtual void runWithWaitClear(InterfaceWatchWaitAndNoneWaitRunCmdItem &i)=0;
    virtual InterfaceWatchWaitAndNoneWaitRunCmdItem *addWithOutWait(std::string uniqueId, std::string fileName, std::string tmpFolder,std::string  vscodePath) =0;
    virtual InterfaceWatchWaitAndNoneWaitRunCmdItem *getNoneWaitItemsBuUiniqueId(std::string uniqueId) = 0;
    virtual void closeWithoutWaitItem(std::string uniqueId) = 0;
    virtual void clearWaitItemsAfterUnExpectedCrash(std::string uniqueId) =0;
};


std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmd>  getInterfaceWatchWaitAndNoneWaitRunCmd();
