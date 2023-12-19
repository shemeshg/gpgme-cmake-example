#pragma once
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include <filesystem>

#include "libpgpfactory.h"
#include "FileSearch.h"
#include "InterfacePassFile.h"
#include <map>


class PassHelper: public GpgFactory, public InterfaceLibgpgfactory
{
public:
  PassHelper()
  {
    initPgpFactory();
  };

  std::vector<GpgKeys> listKeys(const std::string pattern = "",bool secret_only=false) override;
  void setCtxSigners(std::vector<std::string> signedBy)  override;
  virtual void exportPublicKey(std::string const &keyId, std::string const &filePath) override;
  virtual void importPublicKey(std::string const &filePath, bool doTrust) override;
  virtual void trustPublicKey(std::string const &keyId) override;


  std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) override;

  bool isGpgFile(std::string pathToFile) override;

  std::string getNearestGit(std::string currentPath, std::string stopPath) override;

  std::string getNearestGpgId(std::string currentPath, std::string stopPath) override;

  std::string getNearestTemplateGpg(std::string currentPath, std::string stopPath) override;

  void encryptFolderToFolder(std::string folderFrom, std::string folderTo, std::vector<std::string> encryptTo,
        bool doSign) override;

  void decryptFolderToFolder(std::string folderFrom, std::string folderTo) override;

  void searchDown(std::string FolderToSearch,
                  std::string fileRegExStr,
                  std::string contentRegExStr,
                  const std::vector<std::string> &ignoreBinaryExtensions,
                  bool isMemCash,
                  std::map<std::string, std::string> &searchMemCash,
                  std::function<void(std::string s)> callback) override;

  void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo,bool doSign) override;


  void reEncryptStoreFolder(std::string nearestGpgIdFolder, 
  std::vector<std::string> encryptTo,std::function<void (std::string)> func, bool doSign) override;

private:
  FileSearch fileSearch{};


};
