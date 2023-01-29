#pragma once
#include <vector>
#include <string>
#include "libpgpfactory.h"
#include "WatchWaitAndNoneWaitRunCmd.h"
#include <numeric>
#include <thread>

class PassFile
{
public:
  PassFile(std::string fullPath, GpgFactory *g);

  bool isGpgFile();

  void decrypt();

  std::string &getDecrypted();

  std::string const &getFullPath()
  {
    return fullPath;
  }

  std::string const getFullPathFolder()
  {
      if (std::filesystem::is_directory(fullPath))
      {
        return fullPath;

      }
      std::filesystem::path f{fullPath};
      return f.parent_path();

  }

  std::string getDecryptedSignedBy();

  void encrypt(std::string s, std::vector<std::string> encryptTo);

  void openExternalEncryptWait(std::vector<std::string> encryptTo,  WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder);
  void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder);

  std::string openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder);
  void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
          WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);

private:
  std::string fullPath, decrypted;
  std::vector<std::string> decryptedSignedBy = {};  
  GpgFactory *g;
};
