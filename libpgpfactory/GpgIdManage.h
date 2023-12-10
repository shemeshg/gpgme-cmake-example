#pragma once
#include "libpasshelper.h"

class GpgIdManage
{
public:
  GpgIdManage(std::string _currentPath, std::string _stopPath) {
      init(_currentPath,_stopPath);      
  }

  GpgIdManage(GpgIdManage const &) = delete;
  GpgIdManage &operator=(GpgIdManage const &) = delete;
  GpgIdManage(GpgIdManage &&) = delete;
  GpgIdManage &operator=(GpgIdManage &&) = delete;

  ~GpgIdManage(){};

  std::string currentPath, stopPath, nearestGpgIdFolder,
      gpgPubKeysFolder, nearestGpgIdFile;
  bool gpgPubKeysFolderExists = false;
  bool classInitialized = false;

  std::vector<std::string> keysNotFoundInGpgIdFile{};
  std::vector<GpgKeys> keysFoundInGpgIdFile{};
  std::vector<GpgKeys> allKeys;
  std::vector<GpgKeys> allPrivateKeys;
  std::vector<std::string> encryptTo{};

  void importPublicKeyAndTrust(const std::string &filePath);

  void ensureValidGpgIdFile();

  void saveBackGpgIdFile();

  void exportGpgIdToGpgPubKeysFolder();

  void importAllGpgPubKeysFolder();

  void reEncryptFile(std::string pathFileToReEncrypt, bool doSign);

  void reEncryptStoreFolder(std::function<void(std::string)> func, bool doSign);

  void populateKeyFromString(const std::string &line);

  // hygen public

  void setSigner(std::string signerStr){
      if (!signerStr.empty()) {
          ph->setCtxSigners({signerStr});
      }
  }

private:

  void init(std::string _currentPath, std::string _stopPath);

  void populateKeysParsedInGpgIdFile();

  std::string  split(std::string s);


  std::unique_ptr<PassHelper> ph=std::make_unique<PassHelper>();

  // trim from start (in place)
  static inline void ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                  return !std::isspace(ch);
              }));
  }

  // trim from end (in place)
  static inline void rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                  return !std::isspace(ch);
              }).base(), s.end());
  }

  // trim from both ends (in place)
  static inline void trim(std::string &s) {
      rtrim(s);
      ltrim(s);
  }

  // hygen private
};
