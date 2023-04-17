#include "libpasshelper.h"
#include <iostream>
#include <filesystem>


void PassHelper::reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo)
{
    try {
        std::string backupFile = pathFileToReEncrypt + "backup";
        std::filesystem::rename(pathFileToReEncrypt,backupFile);
        PgpmeDataRII ein{backupFile, FROM_FILENAME},
        emem{},
        eout{pathFileToReEncrypt, TO_FILENAME};
        decryptValidate(ein, emem, false);
        emem.getString(); //get fseek to end of buffer
        encryptSign(emem, eout, encryptTo, true);
        std::filesystem::remove(backupFile);
    }
    catch (...){
        std::filesystem::rename(pathFileToReEncrypt + "backup", pathFileToReEncrypt);
        throw;
    }
}
