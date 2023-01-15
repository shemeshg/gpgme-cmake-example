#pragma once
#include "libpgpfactory.h"
#include "FileSearch.h"

class PassFile
{
public:
  PassFile(std::string fullPath, GpgFactory *g);

  bool isGpgFile();

  void decrypt();

  std::string &getDecrypted();

  std::string const &getFullPath(){
    return fullPath;
  }

private:
  std::string fullPath, decrypted;
  GpgFactory *g;
};

class PassHelper
{
public:
  PassHelper()
  {
    g.initPgpFactory();
  };

  std::unique_ptr<PassFile> getPassFile(std::string fullPath)
  {
    return std::make_unique<PassFile>(fullPath, &g);
  }

  GpgFactory g{};

  std::string getNearestGit(std::string currentPath, std::string stopPath )
  {
    return fileSearch.searchUp(".git", currentPath, stopPath);
  }

  std::string getNearestGpgId(std::string currentPath, std::string stopPath )
  {
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
  }  

private:
  FileSearch fileSearch{};
};
