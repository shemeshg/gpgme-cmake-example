#pragma once
#include "InterfaceWatchWaitAndNoneWaitRunCmdItem.h"
#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"
#include "RunShellCmd.h"
#include <mutex>




class WatchWaitAndNoneWaitRunCmd: public InterfaceWatchWaitAndNoneWaitRunCmd
{
public:

    std::mutex g_pages_mutex;

    std::vector<std::string> getNoneWaitItemsUids() override;

    InterfaceWatchWaitAndNoneWaitRunCmdItem *addWithOutWait(std::string uniqueId,
                                                            std::string fileName,
                                                            std::string tmpFolder,
                                                            std::string vscodePath,
                                                            RunShellCmd *rsc) override;

    InterfaceWatchWaitAndNoneWaitRunCmdItem *getNoneWaitItemsBuUiniqueId(std::string uniqueId) override;


    void closeWithoutWaitItem(std::string uniqueId) override;

    void clearWaitItemsAfterUnExpectedCrash(std::string uniqueId) override;

    private:
        std::vector<std::unique_ptr<InterfaceWatchWaitAndNoneWaitRunCmdItem>> noneWaitItems{};
};

