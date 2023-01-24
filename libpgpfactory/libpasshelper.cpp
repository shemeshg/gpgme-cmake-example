#include "libpasshelper.h"
#include <iostream>
#include <filesystem>


void PassHelper::reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo)
{
    std::string backupFile = pathFileToReEncrypt + "backup";
    std::filesystem::rename(pathFileToReEncrypt,backupFile);
    PgpmeDataRII ein{backupFile, FROM_FILENAME},
    emem{},
    eout{pathFileToReEncrypt, TO_FILENAME};
    g.decryptValidate(ein, emem, false);
    emem.getString(); //get fseek to end of buffer
    g.encryptSign(emem, eout, encryptTo, true);
    std::filesystem::remove(backupFile);
}
