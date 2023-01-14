#include "libpasshelper.h"
#include <iostream>
#include <filesystem>

PassFile::PassFile(std::string fullPath, GpgFactory *g):fullPath{fullPath},g{g}
{
}

bool PassFile::isGpgFile(){
    std::filesystem::path path(fullPath);
    return (path.extension().string() == ".gpg");
}

void PassFile::decrypt(){
    PgpmeDataRII din{fullPath,FROM_FILENAME},
    dout{};
    g->decryptValidate(din,dout,false);
    decrypted = dout.getString();
}

std::string &PassFile::getDecrypted(){
    return decrypted;
}
