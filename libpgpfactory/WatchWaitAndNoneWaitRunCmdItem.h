#pragma once
#include <string>
#include <filesystem>
#include "RunShellCmd.h"

class WatchWaitAndNoneWaitRunCmdItem
{
public:
  WatchWaitAndNoneWaitRunCmdItem(std::string uniqueId, std::string fileName, std::string tmpFolder) : uniqueId{uniqueId}, fileName{fileName}, tmpFolder{tmpFolder}
  {
  }

  void init(){
    tfe.init(fileName, tmpFolder);
  }

  void runWithWait()
  {    
    auto a = rsc.runCmd({"/usr/local/bin/code", "--wait", tfe.getFullFilePath()});
  }

  void runWithoutWait()
  {
    auto a = rsc.runCmd({"/usr/local/bin/code", tfe.getFullFilePath()});
  }
  std::string uniqueId, fileName, tmpFolder;

  std::filesystem::path getFullFilePath()
  {
    return tfe.getFullFilePath();
  }

private:
  RunShellCmd rsc{};
  TmpFileWacher tfe{};
};