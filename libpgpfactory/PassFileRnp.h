#pragma once
#include "InterfacePassFile.h"
#include "RnpCoreInterface.h"
#include <string>
#include <vector>

class PassFileRnp : public InterfacePassFile
{
public:
    PassFileRnp(std::string _fullPath, RnpCoreInterface *g)
        : g{g}
    {
        fullPath = _fullPath;
    }

    void decrypt() override {}

    void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign) override {}

    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) override
    {}

    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) override
    {}

    void decryptToFile(std::string toFileName) override {}

    void dectyptFileNameToFileName(std::string fromPath, std::string toPath) override {}

    void reEncryptFile(std::string pathFileToReEncrypt,
                       std::vector<std::string> encryptTo,
                       bool doSign) override
    {}

private:
    RnpCoreInterface *g;

    std::vector<std::string> getPubIdDecryptedSignedBy() override { return {}; }
};