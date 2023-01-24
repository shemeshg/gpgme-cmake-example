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

  std::vector<GpgKeys> listKeys(std::string pattern)
  {
    return g.listKeys(pattern);
  }

  void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo);

private:
  FileSearch fileSearch{};  
};
