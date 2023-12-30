#pragma once
#include "InterfacePassFile.h"
#include "libpgpfactory.h"
#include <string>
#include <vector>

class PassFile : public InterfacePassFile
{
public:
    PassFile(std::string fullPath, GpgFactory *g);

    void decrypt() override;

    void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign) override;

    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) override;
    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) override;
    void decryptToFile(std::string toFileName) override;

    void dectyptFileNameToFileName(std::string fromPath, std::string toPath) override;

    void reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign) override;

private:
    GpgFactory *g;

    std::vector<std::string> getPubIdDecryptedSignedBy() override;
    ;
};
