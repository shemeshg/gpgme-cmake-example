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



    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) override;
                             
    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) override;


    void dectyptFileNameToFileName(std::string fromPath, std::string toPath) override;

    void reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign) override;

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false) override {
        return g->listKeys(pattern, secret_only);
    }

private:
    GpgFactory *g;

   
};
