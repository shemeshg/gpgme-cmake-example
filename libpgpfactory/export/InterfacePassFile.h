#pragma once
#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"
#include "GpgKeys.h"
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

    std::string const getFullPathFolder();

    void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign);

    InterfaceWatchWaitAndNoneWaitRunCmdItem *openExternalEncryptNoWait(
        InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
        std::string tmpFolder,
        std::string vscodePath,
        RunShellCmd *rsc);

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
    void decryptToFile(std::string toFileName) { dectyptFileNameToFileName(fullPath, toFileName); }

    virtual void dectyptFileNameToFileName(std::string fromPath, std::string toPath) = 0;

    virtual void reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign)
        = 0;

    virtual std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false)
        = 0;

    
    virtual bool getIsRnPgp()=0;
    
    void setFullPath(std::string s);

    std::string getDecryptedSignedBy();

    virtual const std::string getRnpHomePath() const {
        return "";
    }
protected:
    std::string fullPath, decrypted;
    std::vector<std::string> decryptedSignedBy = {};

    std::vector<std::string> getPubIdDecryptedSignedBy()
    {
        std::vector<std::string> ret;
        for (const auto &row : decryptedSignedBy) {
            auto svec = listKeys(row);
            if (svec.size() == 1) {
                ret.push_back(svec[0].getKeyStr());
            } else {
                ret.push_back(row);
            }
        }
        return ret;
    }
};
