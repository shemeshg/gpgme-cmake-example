#pragma once
#include "FileSearch.h"
#include "GpgKeys.h"
#include "InterfacePassFile.h"
#include <map>

class InterfaceLibgpgfactory
{
public:
    virtual std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false)
        = 0;
    virtual void setCtxSigners(std::vector<std::string> signedBy) = 0;

    virtual void exportPublicKey(std::string const &keyId, std::string const &filePath) = 0;
    virtual void importPublicKey(std::string const &filePath, bool doTrust) = 0;
    virtual void trustPublicKey(std::string const &keyId) = 0;
    virtual void setPasswordCallback(std::function<std::string(std::string s)> func) = 0;
    virtual ~InterfaceLibgpgfactory() {}

    virtual std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) = 0;
    bool isGpgFile(std::string pathToFile);

    virtual bool useMultiThread()=0;

    std::string getNearestGit(std::string currentPath, std::string stopPath);

    std::string getNearestGpgId(std::string currentPath, std::string stopPath);

    std::string getNearestTemplateGpg(std::string currentPath, std::string stopPath);

    void encryptFolderToFolder(std::string folderFrom,
                               std::string folderTo,
                               std::vector<std::string> encryptTo,
                               bool doSign
                               );

    virtual void decryptFolderToFolder(std::string folderFrom, std::string folderTo);

    virtual void searchDown(std::string FolderToSearch,
                            std::string fileRegExStr,
                            std::string contentRegExStr,
                            bool contentSearchUsingRegEx,
                            const std::vector<std::string> &ignoreBinaryExtensions,
                            const std::vector<std::string> &ignoreSearch,
                            bool isMemCash,
                            std::map<std::string, std::string> &searchMemCash,
                            std::function<void(std::string s)> callback);

    virtual void reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign);

    virtual void reEncryptStoreFolder(std::string nearestGpgIdFolder,
                                      std::vector<std::string> encryptTo,
                                      std::function<void(std::string)> func,
                                      bool doSign);

private:
    FileSearch fileSearch{};
};

InterfaceLibgpgfactory *getInterfacePassHelper(bool isRnPgp, std::string rnpHomePath);
