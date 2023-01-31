#pragma once
#include <string>
#include <filesystem>
#include <iostream>
#include <regex>


class FileSearch
{
public:
  FileSearch() = default;

  FileSearch(FileSearch const &) = delete;
  FileSearch &operator=(FileSearch const &) = delete;
  FileSearch(FileSearch &&) = delete;
  FileSearch &operator=(FileSearch &&) = delete;

  ~FileSearch(){};

  void searchDown(std::string FolderToSearch, 
                std::string fileRegExStr, 
                std::string contentRegExStr,
                 std::function<bool(std::string s)> contentSearch,
                 std::function<void(std::string s)> callback
                )
  {
    const std::regex fileRegEx(fileRegExStr,std::regex_constants::icase);
    

    try
    {
      for (std::filesystem::recursive_directory_iterator it(FolderToSearch); it != end(it); ++it)
      {
        auto entry = *it;
        if (entry.is_directory() && isHidden(entry))
        {
          it.disable_recursion_pending();
          continue;
        }

        if (entry.is_regular_file() && !isHidden(entry))
        {
          std::string path{entry.path()};
          if (std::regex_match(path, fileRegEx) && contentSearch(entry.path()))
          {
            callback(entry.path());            
          }
        }
      }
    }
    catch (std::filesystem::filesystem_error &fe)
    {
    }
  }

  std::string searchUp(std::string fileOrFolderToSearch, std::string currentPath, std::string sstopPath)
  {
    std::filesystem::path pathToDisplay(currentPath);
    std::filesystem::path stopPath(sstopPath);

    if (!std::filesystem::is_directory(pathToDisplay))
    {
      pathToDisplay = pathToDisplay.parent_path();
    }

    bool alreadyCheckStopPath = false;
    while (!alreadyCheckStopPath)
    {
      if (std::filesystem::exists(pathToDisplay.c_str() + std::string("/") + fileOrFolderToSearch))
      {
        return pathToDisplay;
      }
      if ((pathToDisplay == sstopPath) || (pathToDisplay == pathToDisplay.parent_path()))
      {
        alreadyCheckStopPath = true;
      }
      pathToDisplay = pathToDisplay.parent_path();
    };

    return "";
  }

  // hygen public
private:
  // hygen private

  bool isHidden(const std::filesystem::path &p)
  {
    std::string name = p.filename();
    if (name != ".." &&
        name != "." &&
        name[0] == '.')
    {
      return true;
    }
    return false;
  }
};
