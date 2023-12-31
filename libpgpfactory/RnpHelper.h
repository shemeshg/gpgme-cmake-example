#pragma once
#include "GpgKeys.h"
#include "InterfacePassHelper.h"
#include "PassFileRnp.h"
#include "RnpCoreInterface.h"

class RnpHelper : public InterfaceLibgpgfactory
{
public:
    RnpHelper() { rblFactory->initPgpFactory(); }
    ~RnpHelper() {}

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false) override
    {
        std::vector<GpgKeys> v;
        for (const auto &k : rblFactory->listKeys(pattern, secret_only)) {
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
    void setCtxSigners(std::vector<std::string> signedBy) override {
        rblFactory->setCtxSigners(signedBy);
    }

    void exportPublicKey(std::string const &keyId, std::string const &filePath) override {
        rblFactory->exportPublicKey(keyId, filePath);
    }
    void importPublicKey(std::string const &filePath, bool doTrust) override {
        rblFactory->importPublicKey(filePath, doTrust);
    }

    void trustPublicKey(std::string const &keyId) override {}

    std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) override
    {
        return std::make_unique<PassFileRnp>(fullPath, rblFactory.get());
    }

private:
    std::unique_ptr<RnpCoreInterface> rblFactory = getRnpCoreInterface();
};
