#pragma once
#include <string>
#include <filesystem>
#include "RunShellCmd.h"
#include "InterfaceWatchWaitAndNoneWaitRunCmdItem.h"
#include "TmpFileWacher.h"

class WatchWaitAndNoneWaitRunCmdItem: public InterfaceWatchWaitAndNoneWaitRunCmdItem
{
public:
  WatchWaitAndNoneWaitRunCmdItem(std::string uniqueId, 
                std::string fileName, 
                std::string tmpFolder,
                std::string vscodePath) : uniqueId{uniqueId}, fileName{fileName}, tmpFolder{tmpFolder}, vscodePath{vscodePath}
  {
  }

  WatchWaitAndNoneWaitRunCmdItem(WatchWaitAndNoneWaitRunCmdItem const &) = delete;
  WatchWaitAndNoneWaitRunCmdItem &operator=(WatchWaitAndNoneWaitRunCmdItem const &) = delete;
  WatchWaitAndNoneWaitRunCmdItem(WatchWaitAndNoneWaitRunCmdItem &&) = delete;
  WatchWaitAndNoneWaitRunCmdItem &operator=(WatchWaitAndNoneWaitRunCmdItem &&) = delete;


  std::string uniqueId, fileName, tmpFolder, vscodePath;

   const std::string getUniqueId() const override {
    return uniqueId;
   }

  void init() override{
    tfe.init(fileName, tmpFolder);
  }

  void runWithWait() override
  {    
    auto a = rsc.runCmd({vscodePath, "--wait", tfe.getFullFilePath().u8string()});
  }

  void runWithoutWait()
  {
    auto a = rsc.runCmd({vscodePath, tfe.getFullFilePath().u8string()});
  }
  

  std::filesystem::path getFullFilePath() override
  {
    return tfe.getFullFilePath();
  }

  std::filesystem::path getSubfolderPath() override
  {
    return tfe.getSubfolderPath();
  }

private:
  RunShellCmd rsc{};
  TmpFileWacher tfe{};  
};
