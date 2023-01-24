#include "WatchWaitAndNoneWaitRunCmdItem.h"

class WatchWaitAndNoneWaitRunCmd
{
public:
  std::vector<WatchWaitAndNoneWaitRunCmdItem> waitItems{};
  std::vector<WatchWaitAndNoneWaitRunCmdItem> noneWaitItems{};

  std::mutex g_pages_mutex;

  WatchWaitAndNoneWaitRunCmdItem &addWithWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
  {
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
    waitItems.push_back(i);
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
  }

  WatchWaitAndNoneWaitRunCmdItem &addWithOutWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
  {
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
    noneWaitItems.push_back(i);
    noneWaitItems.back().runWithoutWait();
    return noneWaitItems.back();
  }

  void closeWithoutWaitItem(std::string uniqueId)
  {
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    noneWaitItems.erase(std::remove_if(noneWaitItems.begin(),
                                       noneWaitItems.end(),
                                       [&](const WatchWaitAndNoneWaitRunCmdItem itm) -> bool
                                       { return itm.uniqueId == uniqueId; }),
                        noneWaitItems.end());
  }
};
