#pragma once
#include "InterfacePassFile.h"
#include "RnpCoreInterface.h"
#include <string>
#include <vector>

class PassFileRnp : public InterfacePassFile
{
public:
    PassFileRnp(std::string _fullPath, RnpCoreInterface *rbl)
        : rbl{rbl}
    {
        fullPath = _fullPath;
    }

    void decrypt() override { rbl->decryptFileToString(fullPath, decrypted, decryptedSignedBy); }

    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) override
    {
        rbl->encryptSignStringToFile(s, toFileName, encryptTo, doSign);
    }

    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) override
    {
        rbl->encryptSignFileToFile(fromFileName, toFileName, encryptTo, doSign);
    }

    void dectyptFileNameToFileName(std::string fromPath, std::string toPath) override {
        rbl->decryptFileToFile(fromPath, toPath);
    }

    void reEncryptFile(std::string pathFileToReEncrypt,
                       std::vector<std::string> encryptTo,
                       bool doSign) override
    {
       rbl->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign); 
    }

private:
    RnpCoreInterface *rbl;

    std::vector<std::string> getPubIdDecryptedSignedBy() override { return {}; }
};