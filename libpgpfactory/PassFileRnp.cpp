#include "PassFileRnp.h"

PassFileRnp::PassFileRnp(std::string _fullPath, RnpCoreInterface *rbl)
    : rbl{rbl}
{
    fullPath = _fullPath;
}

void PassFileRnp::reEncryptFile(std::string pathFileToReEncrypt,
                                std::vector<std::string> encryptTo,
                                bool doSign)
{
    rbl->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);
}

void PassFileRnp::encryptFileToFile(std::string fromFileName,
                                    std::string toFileName,
                                    std::vector<std::string> encryptTo,
                                    bool doSign)
{
    rbl->encryptSignFileToFile(fromFileName, toFileName, encryptTo, doSign);
}

void PassFileRnp::encryptStringToFile(std::string s,
                                      std::string toFileName,
                                      std::vector<std::string> encryptTo,
                                      bool doSign)
{
    rbl->encryptSignStringToFile(s, toFileName, encryptTo, doSign);
}

void PassFileRnp::decrypt()
{
    rbl->decryptFileToString(fullPath, decrypted, decryptedSignedBy);
}
