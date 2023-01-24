#include "WatchWaitAndNoneWaitRunCmdItem.h"

class WatchWaitAndNoneWaitRunCmd
{
public:
  std::vector<WatchWaitAndNoneWaitRunCmdItem> waitItems{};
  std::vector<WatchWaitAndNoneWaitRunCmdItem> noneWaitItems{};

  WatchWaitAndNoneWaitRunCmdItem &addWithWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
  {
    WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
    waitItems.push_back(i);
    return waitItems.back();
  }

  void runWithWait(WatchWaitAndNoneWaitRunCmdItem &i)
  {
    i.runWithWait();
    waitItems.erase(std::remove_if(waitItems.begin(),
                                   waitItems.end(),
                                   [&](const WatchWaitAndNoneWaitRunCmdItem itm) -> bool
                                   { return itm.uniqueId == i.uniqueId; }),
                    waitItems.end());
  }

  WatchWaitAndNoneWaitRunCmdItem &addWithOutWait(std::string uniqueId, std::string fileName, std::string tmpFolder)
  {
    WatchWaitAndNoneWaitRunCmdItem i{uniqueId, fileName, tmpFolder};
    noneWaitItems.push_back(i);
    noneWaitItems.back().runWithoutWait();
    return noneWaitItems.back();
  }

  void closeWithoutWaitItem(std::string uniqueId)
  {
    noneWaitItems.erase(std::remove_if(noneWaitItems.begin(),
                                       noneWaitItems.end(),
                                       [&](const WatchWaitAndNoneWaitRunCmdItem itm) -> bool
                                       { return itm.uniqueId == uniqueId; }),
                        noneWaitItems.end());
  }
};