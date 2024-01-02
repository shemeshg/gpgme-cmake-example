#pragma once
#include "GpgKeys.h"
#include "InterfacePassHelper.h"
#include "PassFileRnp.h"
#include "RnpCoreInterface.h"

class RnpHelper : public InterfaceLibgpgfactory
{
public:
    RnpHelper(std::string rnpHomePath) { 
        rblFactory = getRnpCoreInterface(rnpHomePath);
        rblFactory->initPgpFactory(); 
    }
    ~RnpHelper() {}

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false) override
    {
        auto pf = getPassFile("");
        return pf->listKeys(pattern, secret_only);
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

    void setPasswordCallback(std::function<std::string(std::string s)> func) override {
        rblFactory->setPasswordCallback(func);
    }
private:
    std::unique_ptr<RnpCoreInterface> rblFactory = nullptr;
};
