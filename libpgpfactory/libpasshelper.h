#pragma once
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
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

  std::string getDecryptedSignedBy(){
    std::ostringstream  imploded;

    return decryptedSignedBy.empty() ? "" : /* leave early if there are no items in the list */
    std::accumulate( /* otherwise, accumulate */
    ++decryptedSignedBy.begin(), decryptedSignedBy.end(), /* the range 2nd to after-last */
    *decryptedSignedBy.begin(), /* and start accumulating with the first item */
    [](auto& a, auto& b) { return a + "," + b; });
   
  }

private:
  std::string fullPath, decrypted;
  std::vector<std::string> decryptedSignedBy = {};
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
