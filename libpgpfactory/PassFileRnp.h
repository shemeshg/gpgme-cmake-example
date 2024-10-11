#pragma once
#include "InterfacePassFile.h"
#include "RnpCoreInterface.h"
#include <string>
#include <vector>

class PassFileRnp : public InterfacePassFile
{
public:
    PassFileRnp(std::string _fullPath, RnpCoreInterface *rbl);

    void decrypt() override;

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false) override
    {
        std::vector<GpgKeys> v;
        for (const auto &k : rbl->listKeys(pattern, secret_only)) {
            GpgKeys gk;
            gk.can_encrypt = k.can_encrypt;
            gk.invalid = k.invalid;
            gk.keyid = k.keyid;
            gk.name = k.name;
            gk.email = k.email;
            gk.foundUsingPattern = k.foundUsingPattern;
            gk.validity = k.validity;
            v.push_back(gk);
        }
        return v;
    }

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

    bool getIsRnPgp() override { return true; }

    const std::string getRnpHomePath() const override{
        return rbl->getRnpHomePath();
    }

private:
    RnpCoreInterface *rbl;
};
