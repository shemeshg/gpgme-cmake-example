#include "PassFile.h"
#include "uuid.h"

PassFile::PassFile(std::string _fullPath, GpgFactory *g)
    : g{g}
{
    fullPath = _fullPath;
}

void PassFile::decrypt()
{
    PgpmeDataRII din{fullPath, FROM_FILENAME}, dout{};
    g->decryptValidate(din, dout, true);
    decrypted = dout.getString();
    decryptedSignedBy = dout.decryptedSignedBy;
}


void PassFile::encryptStringToFile(std::string s,
                                   std::string toFileName,
                                   std::vector<std::string> encryptTo,
                                   bool doSign)
{
    decrypted = s;
    PgpmeDataRII din{s, FROM_STRING}, dout{toFileName, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, doSign);
}

void PassFile::encryptFileToFile(std::string fromFileName,
                                 std::string toFileName,
                                 std::vector<std::string> encryptTo,
                                 bool doSign)
{
    PgpmeDataRII din{fromFileName, FROM_FILENAME}, dout{toFileName, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, doSign);
}



void PassFile::dectyptFileNameToFileName(std::string fromPath, std::string toPath)
{
    PgpmeDataRII din{fromPath, FROM_FILENAME}, dout{toPath, TO_FILENAME};

    g->decryptValidate(din, dout, false);
}

std::vector<std::string> PassFile::getPubIdDecryptedSignedBy()
{
    std::vector<std::string> ret;
    for (const auto &row : decryptedSignedBy) {
        auto svec = g->listKeys(row);
        if (svec.size() == 1) {
            ret.push_back(svec[0].getKeyStr());
        } else {
            ret.push_back(row);
        }
    }
    return ret;
}

void PassFile::reEncryptFile(std::string pathFileToReEncrypt,
                             std::vector<std::string> encryptTo,
                             bool doSign)
{
    try {
        std::string backupFile = pathFileToReEncrypt + "backup";
        std::filesystem::rename(pathFileToReEncrypt, backupFile);
        PgpmeDataRII ein{backupFile, FROM_FILENAME}, emem{}, eout{pathFileToReEncrypt, TO_FILENAME};
        g->decryptValidate(ein, emem, false);
        emem.getString(); //get fseek to end of buffer
        g->encryptSign(emem, eout, encryptTo, doSign);
        std::filesystem::remove(backupFile);
    } catch (...) {
        std::filesystem::rename(pathFileToReEncrypt + "backup", pathFileToReEncrypt);
        throw;
    }    
}
