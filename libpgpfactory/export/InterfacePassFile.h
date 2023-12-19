#pragma once
#include <numeric>
#include <string>
#include <thread>
#include <vector>
#include "WatchWaitAndNoneWaitRunCmd.h"

class InterfacePassFile{
    public:
    virtual ~InterfacePassFile(){}
    virtual bool isGpgFile()=0;

    virtual void decrypt()=0;

    virtual std::string &getDecrypted()=0;

    virtual std::string const &getFullPath() =0;

    virtual std::string const getFullPathFolder() = 0;

    virtual std::string getDecryptedSignedBy()=0;

    virtual void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign)=0;

    virtual void openExternalEncryptWait(std::vector<std::string> encryptTo,
                                 WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                 std::string tmpFolder,
                                 std::string vscodePath,
                                 bool doSign) = 0;
    virtual void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo,
                                      WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                      std::string tmpFolder,
                                      std::string vscodePath,
                                      bool doSign,
                                      std::string signerStr)=0;

    virtual std::string openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                          std::string tmpFolder,
                                          std::string vscodePath) = 0;
    virtual void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
                                    WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                    bool doSign) = 0;

    virtual void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) = 0;
    virtual void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) = 0;
    virtual void decryptToFile(std::string toFileName)=0;

    virtual void dectyptFileNameToFileName(std::string fromPath, std::string toPath) = 0;

    virtual void setFullPath(std::string s) =0;
};