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

  std::vector<std::string> split(std::string s);

  PassHelper *ph;

  // hygen private
};
