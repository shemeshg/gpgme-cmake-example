#pragma once
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include <filesystem>

#include "libpgpfactory.h"
#include "FileSearch.h"
#include "PassFile.h"
#include <map>

class PassHelper: public GpgFactory
{
public:
  PassHelper()
  {
    initPgpFactory();
  };

  
  std::unique_ptr<PassFile> getPassFile(std::string fullPath)
  {
    return std::make_unique<PassFile>(fullPath, this);
  }


  std::string getNearestGit(std::string currentPath, std::string stopPath)
  {
    return fileSearch.searchUp(".git", currentPath, stopPath);
  }

  std::string getNearestGpgId(std::string currentPath, std::string stopPath)
  {
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
  }

  std::string getNearestTemplateGpg(std::string currentPath, std::string stopPath)
  {
    return fileSearch.searchUp("template.gpg", currentPath, stopPath);
  }

  void encryptFolderToFolder(std::string folderFrom, std::string folderTo, std::vector<std::string> encryptTo)
  {
    std::unique_ptr<PassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom, ".*.*", ".*.*",
        [&](std::string path)
        {
          return true;
        },
        [&](std::string path)
        {
          std::filesystem::path toPath = std::filesystem::path(folderTo) /
                                         std::filesystem::relative(path, folderFrom);

          std::filesystem::create_directories(toPath.parent_path());
          pf->encryptFileToFile(path, toPath.generic_string() + ".gpg",encryptTo);
          
          return true;
        });
  }




  void decryptFolderToFolder(std::string folderFrom, std::string folderTo)
  {
    std::unique_ptr<PassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom, ".*.*", ".*.*",
        [&](std::string path)
        {
          return true;
        },
        [&](std::string path)
        {
          std::filesystem::path toPath = std::filesystem::path(folderTo) /
                                         std::filesystem::relative(path, folderFrom);

          std::filesystem::create_directories(toPath.parent_path());
          pf->setFullPath(path);
          pf->decryptToFile(toPath.replace_extension());
          std::cout << path << " \n to" << toPath.parent_path() << "\n";
          return true;
        });
  }

  void searchDown(std::string FolderToSearch,
                  std::string fileRegExStr,
                  std::string contentRegExStr,
                  const std::vector<std::string> &ignoreBinaryExtensions,
                  bool isMemCash,
                  std::map<std::string, std::string> &searchMemCash,
                  std::function<void(std::string s)> callback)
  {
    if (!isMemCash){
        searchMemCash.clear();
    }
    std::unique_ptr<PassFile> pf = getPassFile("");
    fileSearch.searchDown(
        FolderToSearch, fileRegExStr, contentRegExStr,
        [&](std::string path)
        {
          if (contentRegExStr == ".*.*")
          {
            return true;
          }

          std::filesystem::path filePath(path);

          if (std::find(ignoreBinaryExtensions.begin(), ignoreBinaryExtensions.end(),
                filePath.stem().extension()) != ignoreBinaryExtensions.end()){
            return false;
          }

          std::string content;
          bool foundInCash = false;
          long int millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::filesystem::last_write_time(filePath).time_since_epoch()
                                ).count();

          std::string memCashKey = std::to_string( millis) + path;
          if (isMemCash) {
            if (searchMemCash.count(memCashKey) > 0) {
                content = searchMemCash[memCashKey];
                foundInCash = true;
            }
          }

          if (!foundInCash) {
            pf->setFullPath(path);

            if (!pf->isGpgFile())
            {
                return false;
            }
            try {
                pf->decrypt();

                content = pf->getDecrypted();
                if (isMemCash) {
                    searchMemCash[memCashKey] = content;
                }
            } catch (const std::exception &e) {
                throw
                    std::runtime_error(path + ": " + e.what());
            }


          }

          const std::regex contentRegEx(contentRegExStr, std::regex_constants::icase);

          content = std::regex_replace(content,
                                       std::regex("\\r\\n|\\r|\\n"),
                                       "");

          bool a = std::regex_match(content, contentRegEx);

          return a;
        },
        callback);
  }

  void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo,bool doSign);

private:
  FileSearch fileSearch{};


};
