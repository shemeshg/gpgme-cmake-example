#include "PassFile.h"
#include "WatchWaitAndNoneWaitRunCmdItem.h"


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
    decryptedSignedBy = dout.decryptedSignedBy;
}

std::string &PassFile::getDecrypted(){
    return decrypted;
}

std::string PassFile::getDecryptedSignedBy()
{
    std::ostringstream imploded;

    return decryptedSignedBy.empty() ? "" :                                          /* leave early if there are no items in the list */
                                       std::accumulate(                                                      /* otherwise, accumulate */
                                                                                                             ++decryptedSignedBy.begin(), decryptedSignedBy.end(), /* the range 2nd to after-last */
                                                                                                             *decryptedSignedBy.begin(),                           /* and start accumulating with the first item */
                                                                                                             [](auto &a, auto &b)
    { return a + "," + b; });
}

void PassFile::encrypt(std::string s, std::vector<std::string> encryptTo){
    decrypted = s;
    PgpmeDataRII din{s,FROM_STRING}, dout{fullPath,TO_FILENAME};
    g->encryptSign(din,dout,encryptTo,true);
}

void PassFile::openExternalEncryptWait(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd){

    std::filesystem::path p = fullPath;
    p = p.replace_extension();
    WatchWaitAndNoneWaitRunCmdItem wi = watchWaitAndNoneWaitRunCmd->addWithWait(fullPath,p.filename(),"/Volumes/RAM_Disk_4G/tmp");
    wi.init();
    PgpmeDataRII din{fullPath,FROM_FILENAME}, dout{wi.getFullFilePath(),TO_FILENAME};

    g->decryptValidate(din,dout,false);
    din.closeFiles();
    dout.closeFiles();

    wi.runWithWait();
    std::cout<<"from "<<fullPath<<" to "<< wi.getFullFilePath()<<"\n";
    PgpmeDataRII ein{wi.getFullFilePath(),FROM_FILENAME};
    PgpmeDataRII eout{fullPath,TO_FILENAME};
    g->encryptSign(din,dout,encryptTo,true);
}
