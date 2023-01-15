#pragma once
#include <string>
#include <filesystem>
#include <iostream>

class FileSearch
{
public:
  FileSearch() = default;

  FileSearch(FileSearch const &) = delete;
  FileSearch &operator=(FileSearch const &) = delete;
  FileSearch(FileSearch &&) = delete;
  FileSearch &operator=(FileSearch &&) = delete;

  ~FileSearch(){};

  std::string searchUp(std::string fileOrFolderToSearch, std::string currentPath, std::string sstopPath)
  {
    std::filesystem::path pathToDisplay(currentPath);
    std::filesystem::path stopPath(sstopPath);

    if (!std::filesystem::is_directory(pathToDisplay))
    {
      pathToDisplay = pathToDisplay.parent_path();      
    }

  bool alreadyCheckStopPath = false;
   while (!alreadyCheckStopPath){
        if (std::filesystem::exists(pathToDisplay.c_str() + std::string("/")+fileOrFolderToSearch)){
          return pathToDisplay;
        }
        if ( (pathToDisplay == sstopPath) || (pathToDisplay == pathToDisplay.parent_path())){
          alreadyCheckStopPath = true;
        }
        pathToDisplay = pathToDisplay.parent_path();
    };

    return "";
  }

  // hygen public
private:
  // hygen private
};
