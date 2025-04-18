#pragma once
#include <functional>
#include <string>
#include <vector>

#include "InterfacePassFile.h"
#include "libpgpfactory.h"

#include "InterfacePassHelper.h"

class PassHelper : public InterfaceLibgpgfactory
{
public:
    PassHelper() { gpgFactory->initPgpFactory(); };

    std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false) override;
    void setCtxSigners(std::vector<std::string> signedBy) override;
    virtual void exportPublicKey(std::string const &keyId, std::string const &filePath) override;
    virtual void importPublicKey(std::string const &filePath, bool doTrust) override;
    virtual void trustPublicKey(std::string const &keyId) override;

    std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) override;

    void setPasswordCallback(std::function<std::string(std::string s)> func) override {}

    bool useMultiThread() override {
        return false;
    }
    void setUseMultiThread(bool isMultiThread) override{
        return;
    }
private:
    std::unique_ptr<GpgFactory> gpgFactory = std::make_unique<GpgFactory>();
};
