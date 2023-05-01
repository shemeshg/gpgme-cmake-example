#include "WatchWaitAndNoneWaitRunCmdItem.h"
#include <algorithm>

class WatchWaitAndNoneWaitRunCmd
{
public:
    std::vector<std::unique_ptr<WatchWaitAndNoneWaitRunCmdItem>> waitItems{};
    std::vector<std::unique_ptr<WatchWaitAndNoneWaitRunCmdItem>> noneWaitItems{};

    std::function<void()> callback=[&](){};

    std::mutex g_pages_mutex;

    WatchWaitAndNoneWaitRunCmdItem *addWithWait(std::string uniqueId, std::string fileName, std::string tmpFolder,std::string  vscodePath)
    {

        std::lock_guard<std::mutex> guard(g_pages_mutex);
        waitItems.push_back(std::make_unique<WatchWaitAndNoneWaitRunCmdItem>(uniqueId, fileName, tmpFolder, vscodePath));
        callback();
        return waitItems.back().get();
    }

    void runWithWaitClear(WatchWaitAndNoneWaitRunCmdItem &i)
    {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        for (auto it = waitItems.begin(); it != waitItems.end();)
        {
            if (it->get()->uniqueId == i.uniqueId)
                it = waitItems.erase(it);
            else
                ++it;
        }
        callback();
    }

    WatchWaitAndNoneWaitRunCmdItem *addWithOutWait(std::string uniqueId, std::string fileName, std::string tmpFolder,std::string  vscodePath)
    {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        noneWaitItems.push_back(std::make_unique<WatchWaitAndNoneWaitRunCmdItem>(uniqueId, fileName, tmpFolder, vscodePath));
        callback();

        return noneWaitItems.back().get();
    }

    WatchWaitAndNoneWaitRunCmdItem *getNoneWaitItemsBuUiniqueId(std::string uniqueId){
        for (auto it = noneWaitItems.begin(); it != noneWaitItems.end();)
        {
            if (it->get()->uniqueId == uniqueId){
                return it->get();
            }
        }
        return nullptr;
    }


    void closeWithoutWaitItem(std::string uniqueId)
    {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        for (auto it = noneWaitItems.begin(); it != noneWaitItems.end();)
        {
            if (it->get()->uniqueId == uniqueId)
                it = noneWaitItems.erase(it);
            else
                ++it;
        }

        callback();
    }

    void clearWaitItemsAfterUnExpectedCrash(std::string uniqueId){
        std::cout<<" Clearing after crash"<<uniqueId;
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        for (auto it = noneWaitItems.begin(); it != noneWaitItems.end();)
        {
            if (it->get()->uniqueId == uniqueId)
                it = noneWaitItems.erase(it);
            else
                ++it;
        }

        for (auto it = waitItems.begin(); it != waitItems.end();)
        {
            if (it->get()->uniqueId == uniqueId)
                it = waitItems.erase(it);
            else
                ++it;
        }

        callback();
    }
};
