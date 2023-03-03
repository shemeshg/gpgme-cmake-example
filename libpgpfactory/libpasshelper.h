#pragma once
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include <filesystem>

#include "libpgpfactory.h"
#include "FileSearch.h"
#include "RunShellCmd.h"
#include "PassFile.h"

class PassHelper
{
public:
  PassHelper()
  {
    g.initPgpFactory();
  };

  GpgFactory g{};

  void setCtxSigner(std::vector<std::string> signedBy)
  {
    g.setCtxSigners(signedBy);
  }

  std::unique_ptr<PassFile> getPassFile(std::string fullPath)
  {
    return std::make_unique<PassFile>(fullPath, &g);
  }

  void exportPublicKey(const std::string &keyId, const std::string &filePath)
  {
    g.exportPublicKey(keyId, filePath);
  }

  void importPublicKey(const std::string &filePath, bool doTrust)
  {
    g.importPublicKey(filePath, doTrust);
  }

  void trustPublicKey(std::string const &keyId)
  {
    g.trustPublicKey(keyId);
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
    PassFile pf = PassFile("", &g);
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
          pf.encryptFileToFile(path, toPath.generic_string() + ".gpg",encryptTo);
          
          return true;
        });
  }




  void decryptFolderToFolder(std::string folderFrom, std::string folderTo)
  {
    PassFile pf = PassFile("", &g);
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
          pf.setFullPath(path);
          pf.decryptToFile(toPath.replace_extension());
          std::cout << path << " \n to" << toPath.parent_path() << "\n";
          return true;
        });
  }

  void searchDown(std::string FolderToSearch,
                  std::string fileRegExStr,
                  std::string contentRegExStr,
                  std::function<void(std::string s)> callback)
  {
    PassFile pf = PassFile("", &g);
    fileSearch.searchDown(
        FolderToSearch, fileRegExStr, contentRegExStr,
        [&](std::string path)
        {
          if (contentRegExStr == ".*.*")
          {
            return true;
          }
          pf.setFullPath(path);

          if (!pf.isGpgFile())
          {
            return false;
          }
          pf.decrypt();

          std::string content = pf.getDecrypted();
          const std::regex contentRegEx(contentRegExStr, std::regex_constants::icase);

          content = std::regex_replace(content,
                                       std::regex("\\r\\n|\\r|\\n"),
                                       "");

          bool a = std::regex_match(content, contentRegEx);

          return a;
        },
        callback);
  }

  std::vector<GpgKeys> listKeys(std::string pattern, bool secret_only = false)
  {
    return g.listKeys(pattern, secret_only);
  }

  void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo);

private:
  FileSearch fileSearch{};
};
