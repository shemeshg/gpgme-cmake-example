#include "libpasshelper.h"
#include "PassFile.h"

std::vector<GpgKeys> PassHelper::listKeys(const std::string pattern, bool secret_only)
{
    return gpgFactory->listKeys(pattern, secret_only);
}

void PassHelper::setCtxSigners(std::vector<std::string> signedBy)
{
    return gpgFactory->setCtxSigners(signedBy);
}

void PassHelper::exportPublicKey(const std::string &keyId, const std::string &filePath)
{
    return gpgFactory->exportPublicKey(keyId, filePath);
}

void PassHelper::importPublicKey(const std::string &filePath, bool doTrust)
{
    return gpgFactory->importPublicKey(filePath, doTrust);
}

void PassHelper::trustPublicKey(const std::string &keyId)
{
    return gpgFactory->trustPublicKey(keyId);
}

std::unique_ptr<InterfacePassFile> PassHelper::getPassFile(std::string fullPath)
{
    return std::make_unique<PassFile>(fullPath, gpgFactory.get());
}
