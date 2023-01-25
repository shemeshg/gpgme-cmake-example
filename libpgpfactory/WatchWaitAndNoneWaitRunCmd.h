#include "WatchWaitAndNoneWaitRunCmdItem.h"

class WatchWaitAndNoneWaitRunCmd
{
public:
    std::vector<WatchWaitAndNoneWaitRunCmdItem> waitItems{};
    std::vector<WatchWaitAndNoneWaitRunCmdItem> noneWaitItems{};

    std::function<void()> callback=[&](){};

    std::mutex g_pages_mutex;

    WatchWaitAndNoneWaitRunCmdItem &addWithWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
    {

        std::lock_guard<std::mutex> guard(g_pages_mutex);
        WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
        waitItems.push_back(i);
        callback();
        return waitItems.back();
    }

    void runWithWait(WatchWaitAndNoneWaitRunCmdItem &i)
    {
        i.runWithWait();
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        waitItems.erase(std::remove_if(waitItems.begin(),
                                       waitItems.end(),
                                       [&](const WatchWaitAndNoneWaitRunCmdItem itm) -> bool
        { return itm.uniqueId == i.uniqueId; }),
                        waitItems.end());
        callback();
    }

    WatchWaitAndNoneWaitRunCmdItem *addWithOutWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
    {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
        noneWaitItems.push_back(i);
        callback();

        return &noneWaitItems.back();
    }

    WatchWaitAndNoneWaitRunCmdItem *getNoneWaitItemsBuUiniqueId(std::string uniqueId){
        auto it = find_if(noneWaitItems.begin(), noneWaitItems.end(),
                          [&uniqueId](const WatchWaitAndNoneWaitRunCmdItem& obj) {return obj.uniqueId == uniqueId;});

        // assume if (it != v.end()){
        auto index = std::distance(noneWaitItems.begin(), it);
        return &noneWaitItems.at(index);
    }


    void closeWithoutWaitItem(std::string uniqueId)
    {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        noneWaitItems.erase(std::remove_if(noneWaitItems.begin(),
                                           noneWaitItems.end(),
                                           [&](const WatchWaitAndNoneWaitRunCmdItem itm) -> bool
        { return itm.uniqueId == uniqueId; }),
                            noneWaitItems.end());
        callback();
    }
};
