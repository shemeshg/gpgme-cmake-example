#pragma once
#include "libpasshelper.h"

class GpgIdManage
{
public:
  GpgIdManage() {}

  GpgIdManage(GpgIdManage const &) = delete;
  GpgIdManage &operator=(GpgIdManage const &) = delete;
  GpgIdManage(GpgIdManage &&) = delete;
  GpgIdManage &operator=(GpgIdManage &&) = delete;

  ~GpgIdManage(){};

  std::string currentPath, stopPath, nearestGpgIdFolder,
      gpgPubKeysFolder, nearestGpgIdFile;
  bool gpgPubKeysFolderExists = false;
  bool classInitialized = false;

  std::vector<std::string> KeysNotFoundInGpgIdFile{};
  std::vector<GpgKeys> keysFoundInGpgIdFile{};
  std::vector<GpgKeys> allKeys;
  std::vector<GpgKeys> allPrivateKeys;
  std::vector<std::string> encryptTo{};

  void init(std::string _currentPath, std::string _stopPath, PassHelper *_ph);

  void reInit(){
      init(currentPath,stopPath,ph);
  }

  void importPublicKeyAndTrust(const std::string &filePath);

  void ensureValidGpgIdFile();

  void saveBackGpgIdFile();

  void exportGpgIdToGpgPubKeysFolder();

  void importAllGpgPubKeysFolder();

  void reEncryptFile(std::string pathFileToReEncrypt);

  void reEncryptStoreFolder(std::function<void(std::string)> func);

  void populateKeyFromString(const std::string &line);

  // hygen public
private:
  void populateKeysParsedInGpgIdFile();

  std::string  split(std::string s);

  PassHelper *ph;

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
