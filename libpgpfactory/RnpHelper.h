#pragma once
#include "GpgKeys.h"
#include "PassFileRnp.h"
#include "InterfacePassHelper.h"
#include "RnpCoreInterface.h"

class RnpHelper : public InterfaceLibgpgfactory
{
public:
    RnpHelper() { rblFactory->initPgpFactory(); }
    ~RnpHelper() {}

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false)
    {
        return {};
    }
    void setCtxSigners(std::vector<std::string> signedBy) {}

    void exportPublicKey(std::string const &keyId, std::string const &filePath) {}
    void importPublicKey(std::string const &filePath, bool doTrust) {}
    void trustPublicKey(std::string const &keyId) {}

    std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath)
    {
        return std::make_unique<PassFileRnp>(fullPath, rblFactory.get());
    }
    
private:
    std::unique_ptr<RnpCoreInterface> rblFactory = getRnpCoreInterface();
};
