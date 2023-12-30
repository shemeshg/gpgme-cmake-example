#include "PassFile.h"
#include "WatchWaitAndNoneWaitRunCmdItem.h"
#include "libpasshelper.h"

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

void PassFile::encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign)
{
    std::string tmpName = fullPath + uuid::generate_uuid_v4();

    decrypted = s;
    PgpmeDataRII din{s, FROM_STRING}, dout{tmpName, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, doSign);

    std::filesystem::rename(tmpName, fullPath);
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

void PassFile::decryptToFile(std::string toFileName)
{
    PgpmeDataRII din{fullPath, FROM_FILENAME}, dout{toFileName, TO_FILENAME};
    g->decryptValidate(din, dout, false);
}

void PassFile::dectyptFileNameToFileName(std::string fromPath, std::string toPath)
{
    PgpmeDataRII din{fromPath, FROM_FILENAME}, dout{toPath, TO_FILENAME};

    g->decryptValidate(din, dout, false);
}


