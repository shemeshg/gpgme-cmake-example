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

  void openExternalEncryptWait(std::vector<std::string> encryptTo,  WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder, std::string  vscodePath);
  void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder, std::string  vscodePath);

  std::string openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder, std::string  vscodePath);
  void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
          WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd);

  void encryptStringToFile(std::string s, std::string toFileName, std::vector<std::string> encryptTo);
  void encryptFileToFile(std::string fromFileName, std::string toFileName, std::vector<std::string> encryptTo);
  void decryptToFile(std::string toFileName);

  void setFullPath(std::string s){
    fullPath = s;
  }
private:
  std::string fullPath, decrypted;
  std::vector<std::string> decryptedSignedBy = {};  
  GpgFactory *g;
};
