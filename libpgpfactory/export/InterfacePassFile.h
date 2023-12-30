#pragma once
#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"
#include <string>
#include <vector>

class InterfacePassFile
{
public:
    virtual ~InterfacePassFile() {}
    bool isGpgFile();

    virtual void decrypt() = 0;

    std::string &getDecrypted() { return decrypted; }

    std::string const &getFullPath() { return fullPath; }

    std::string const getFullPathFolder()
    {
        if (std::filesystem::is_directory(fullPath)) {
            return fullPath;
        }
        std::filesystem::path f{fullPath};
        return f.parent_path().u8string();
    }

    virtual void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign) = 0;

    virtual void openExternalEncryptWait(
        std::vector<std::string> encryptTo,
        InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
        std::string tmpFolder,
        std::string vscodePath,
        bool doSign);

    void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo,
                                      InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                      std::string tmpFolder,
                                      std::string vscodePath,
                                      bool doSign,
                                      std::string signerStr);

    std::string openExternalEncryptNoWait(
        InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
        std::string tmpFolder,
        std::string vscodePath);

    void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
                                    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                    bool doSign);

    virtual void encryptStringToFile(std::string s,
                                     std::string toFileName,
                                     std::vector<std::string> encryptTo,
                                     bool doSign)
        = 0;
    virtual void encryptFileToFile(std::string fromFileName,
                                   std::string toFileName,
                                   std::vector<std::string> encryptTo,
                                   bool doSign)
        = 0;
    virtual void decryptToFile(std::string toFileName) = 0;

    virtual void dectyptFileNameToFileName(std::string fromPath, std::string toPath) = 0;

    void setFullPath(std::string s);

    std::string getDecryptedSignedBy();

protected:
    std::string fullPath, decrypted;
    std::vector<std::string> decryptedSignedBy = {};

private:
    virtual std::vector<std::string> getPubIdDecryptedSignedBy() = 0;
};
