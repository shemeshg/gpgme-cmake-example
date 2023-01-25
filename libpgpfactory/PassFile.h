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

  std::string getDecryptedSignedBy();

  void encrypt(std::string s, std::vector<std::string> encryptTo);

  void openExternalEncryptWait(std::vector<std::string> encryptTo,  WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);
  void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);

  std::string openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);
  void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
          WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);

private:
  std::string fullPath, decrypted;
  std::vector<std::string> decryptedSignedBy = {};  
  GpgFactory *g;
};