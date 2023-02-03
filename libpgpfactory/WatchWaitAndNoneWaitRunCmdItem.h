#pragma once
#include <string>
#include <filesystem>
#include "RunShellCmd.h"

class WatchWaitAndNoneWaitRunCmdItem
{
public:
  WatchWaitAndNoneWaitRunCmdItem(std::string uniqueId, 
                std::string fileName, 
                std::string tmpFolder,
                std::string vscodePath) : uniqueId{uniqueId}, fileName{fileName}, tmpFolder{tmpFolder}, vscodePath{vscodePath}
  {
  }

  std::string uniqueId, fileName, tmpFolder, vscodePath;

  void init(){
    tfe.init(fileName, tmpFolder);
  }

  void runWithWait()
  {    
    auto a = rsc.runCmd({vscodePath, "--wait", tfe.getFullFilePath()});
  }

  void runWithoutWait()
  {
    auto a = rsc.runCmd({vscodePath, tfe.getFullFilePath()});
  }
  

  std::filesystem::path getFullFilePath()
  {
    return tfe.getFullFilePath();
  }

  std::filesystem::path getSubfolderPath()
  {
    return tfe.getSubfolderPath();
  }

private:
  RunShellCmd rsc{};
  TmpFileWacher tfe{};  
};
