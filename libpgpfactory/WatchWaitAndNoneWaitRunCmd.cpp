#include "WatchWaitAndNoneWaitRunCmd.h"
#include "WatchWaitAndNoneWaitRunCmdItem.h"

std::vector<std::string> WatchWaitAndNoneWaitRunCmd::getWaitItemsUids() {
    std::vector<std::string>  waitString{};
    for (auto &itm : waitItems) {
        waitString.push_back(itm->getUniqueId());
    }
    return waitString;
}

std::vector<std::string> WatchWaitAndNoneWaitRunCmd::getNoneWaitItemsUids() {
    std::vector<std::string>  noneWaitString{};
    for (auto &itm : noneWaitItems) {
        noneWaitString.push_back(itm->getUniqueId());
    }
    return noneWaitString;
}

InterfaceWatchWaitAndNoneWaitRunCmdItem *WatchWaitAndNoneWaitRunCmd::addWithWait(std::string uniqueId, std::string fileName, 
    std::string tmpFolder, std::string vscodePath, RunShellCmd *rsc)
{

    std::lock_guard<std::mutex> guard(g_pages_mutex);
    waitItems.push_back(std::make_unique<WatchWaitAndNoneWaitRunCmdItem>(uniqueId, fileName, tmpFolder, vscodePath, rsc));
    callback();
    return waitItems.back().get();
}

void WatchWaitAndNoneWaitRunCmd::runWithWaitClear(InterfaceWatchWaitAndNoneWaitRunCmdItem &i)
{
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    for (auto it = waitItems.begin(); it != waitItems.end();)
    {
        if (it->get()->getUniqueId()  == i.getUniqueId() )
            it = waitItems.erase(it);
        else
            ++it;
    }
    callback();
}

InterfaceWatchWaitAndNoneWaitRunCmdItem *WatchWaitAndNoneWaitRunCmd::getNoneWaitItemsBuUiniqueId(std::string uniqueId){
    for ( auto &it: noneWaitItems)
    {
        if (it.get()->getUniqueId() == uniqueId){
            return it.get();
        }
    }
    return nullptr;
}

void WatchWaitAndNoneWaitRunCmd::closeWithoutWaitItem(std::string uniqueId)
{
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    for (auto it = noneWaitItems.begin(); it != noneWaitItems.end();)
    {
        if (it->get()->getUniqueId() == uniqueId)
            it = noneWaitItems.erase(it);
        else
            ++it;
    }

    callback();
}

void WatchWaitAndNoneWaitRunCmd::clearWaitItemsAfterUnExpectedCrash(std::string uniqueId){
    //std::cout<<" Clearing after crash"<<uniqueId;
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    for (auto it = noneWaitItems.begin(); it != noneWaitItems.end();)
    {
        if (it->get()->getUniqueId() == uniqueId)
            it = noneWaitItems.erase(it);
        else
            ++it;
    }

    for (auto it = waitItems.begin(); it != waitItems.end();)
    {
        if (it->get()->getUniqueId() == uniqueId)
            it = waitItems.erase(it);
        else
            ++it;
    }

    callback();
}

InterfaceWatchWaitAndNoneWaitRunCmdItem *WatchWaitAndNoneWaitRunCmd::addWithOutWait(std::string uniqueId, std::string fileName, 
    std::string tmpFolder, std::string vscodePath,RunShellCmd *rsc)
{
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    noneWaitItems.push_back(std::make_unique<WatchWaitAndNoneWaitRunCmdItem>(uniqueId, fileName, tmpFolder, vscodePath, rsc));
    callback();

    return noneWaitItems.back().get();
}
