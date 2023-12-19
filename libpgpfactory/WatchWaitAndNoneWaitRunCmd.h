#pragma once
#include "InterfaceWatchWaitAndNoneWaitRunCmdItem.h"
#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"





class WatchWaitAndNoneWaitRunCmd: public InterfaceWatchWaitAndNoneWaitRunCmd
{
public:

    std::mutex g_pages_mutex;

    std::vector<std::string> getWaitItemsUids() override;

    std::vector<std::string> getNoneWaitItemsUids() override;


    InterfaceWatchWaitAndNoneWaitRunCmdItem *addWithWait(std::string uniqueId,
        std::string fileName, std::string tmpFolder,std::string  vscodePath) override;

    void runWithWaitClear(InterfaceWatchWaitAndNoneWaitRunCmdItem &i) override;

    InterfaceWatchWaitAndNoneWaitRunCmdItem *addWithOutWait(std::string uniqueId,
        std::string fileName, std::string tmpFolder,std::string  vscodePath) override;

    InterfaceWatchWaitAndNoneWaitRunCmdItem *getNoneWaitItemsBuUiniqueId(std::string uniqueId) override;


    void closeWithoutWaitItem(std::string uniqueId) override;

    void clearWaitItemsAfterUnExpectedCrash(std::string uniqueId) override;

    private:
        std::vector<std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmdItem>> waitItems{};
        std::vector<std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmdItem>> noneWaitItems{};
};

