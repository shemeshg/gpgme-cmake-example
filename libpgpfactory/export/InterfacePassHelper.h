#pragma once
#include <map>
#include "GpgKeys.h"
#include "InterfacePassFile.h"

class InterfaceLibgpgfactory{
public:

    virtual std::vector<GpgKeys> listKeys(const std::string pattern = "",bool secret_only=false)=0;
    virtual void setCtxSigners(std::vector<std::string> signedBy) = 0;

  virtual void exportPublicKey(std::string const &keyId, std::string const &filePath) = 0;
  virtual void importPublicKey(std::string const &filePath, bool doTrust) = 0;
  virtual void trustPublicKey(std::string const &keyId) = 0;  

    virtual ~InterfaceLibgpgfactory(){}

virtual std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) = 0;
  virtual bool isGpgFile(std::string pathToFile) = 0;

  virtual std::string getNearestGit(std::string currentPath, std::string stopPath) = 0;

  virtual std::string getNearestGpgId(std::string currentPath, std::string stopPath) = 0;

  virtual std::string getNearestTemplateGpg(std::string currentPath, std::string stopPath) = 0;

  virtual void encryptFolderToFolder(std::string folderFrom, std::string folderTo, std::vector<std::string> encryptTo,
        bool doSign) = 0;

  virtual void decryptFolderToFolder(std::string folderFrom, std::string folderTo) = 0;

  virtual void searchDown(std::string FolderToSearch,
                  std::string fileRegExStr,
                  std::string contentRegExStr,
                  const std::vector<std::string> &ignoreBinaryExtensions,
                  bool isMemCash,
                  std::map<std::string, std::string> &searchMemCash,
                  std::function<void(std::string s)> callback) = 0;

  virtual void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo,bool doSign) = 0;


  virtual void reEncryptStoreFolder(std::string nearestGpgIdFolder, std::vector<std::string> encryptTo,std::function<void (std::string)> func, bool doSign) = 0;


};




std::unique_ptr<InterfaceLibgpgfactory>  getInterfacePassHelper();


