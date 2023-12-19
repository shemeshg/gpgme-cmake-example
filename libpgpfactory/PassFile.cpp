#include "PassFile.h"
#include "WatchWaitAndNoneWaitRunCmdItem.h"
#include "libpasshelper.h"

PassFile::PassFile(std::string fullPath, GpgFactory *g) : fullPath{fullPath}, g{g}
{
}

bool PassFile::isGpgFile()
{
    std::filesystem::path path(fullPath);
    return (!std::filesystem::is_directory(path) && path.extension().string() == ".gpg");
}

void PassFile::decrypt()
{
    PgpmeDataRII din{fullPath, FROM_FILENAME},
        dout{};
    g->decryptValidate(din, dout, true);
    decrypted = dout.getString();
    decryptedSignedBy = dout.decryptedSignedBy;
}

std::string &PassFile::getDecrypted()
{
    return decrypted;
}

std::string PassFile::getDecryptedSignedBy()
{
    std::ostringstream imploded;
    auto vec=getPubIdDecryptedSignedBy();
    return decryptedSignedBy.empty() ? "" :                                          /* leave early if there are no items in the list */
               std::accumulate(                                                      /* otherwise, accumulate */
                               ++vec.begin(), vec.end(), /* the range 2nd to after-last */
                               *vec.begin(),                           /* and start accumulating with the first item */
                               [](auto &a, auto &b)
                               { return a + "," + b; });
}

void PassFile::encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign)
{
    std::string tmpName = fullPath + uuid::generate_uuid_v4();

    decrypted = s;
    PgpmeDataRII din{s, FROM_STRING}, dout{tmpName, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, doSign);

    std::filesystem::rename(tmpName, fullPath);

}

void PassFile::encryptStringToFile(std::string s, std::string toFileName, std::vector<std::string> encryptTo,
        bool doSign)
{
    decrypted = s;
    PgpmeDataRII din{s, FROM_STRING}, dout{toFileName, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, doSign);
}

void PassFile::encryptFileToFile(std::string fromFileName, std::string toFileName,
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

void PassFile::dectyptFileNameToFileName(std::string fromPath, std::string toPath){
    PgpmeDataRII din{fromPath, FROM_FILENAME}, dout{toPath, TO_FILENAME};

    g->decryptValidate(din, dout, false);
}


void PassFile::openExternalEncryptWait(std::vector<std::string> encryptTo, 
                            InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                       std::string tmpFolder,std::string  vscodePath,
                                    bool doSign)
{
    try
    {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        InterfaceWatchWaitAndNoneWaitRunCmdItem *wi = watchWaitAndNoneWaitRunCmd->addWithWait(fullPath, p.filename().u8string(), tmpFolder, vscodePath);

        wi->init();
        dectyptFileNameToFileName(fullPath, wi->getFullFilePath().u8string());

        std::string ggg=wi->getFullFilePath().u8string();
        wi->runWithWait();
        PgpmeDataRII ein{wi->getFullFilePath().u8string(), FROM_FILENAME};
        PgpmeDataRII eout{fullPath, TO_FILENAME};
        g->encryptSign(ein, eout, encryptTo, doSign);
        watchWaitAndNoneWaitRunCmd->runWithWaitClear(*wi);
    }
    catch (const std::exception &e)
    {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void PassFile::openExternalEncryptWaitAsync(std::vector<std::string> encryptTo, 
                                        InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                            std::string tmpFolder,
                                            std::string  vscodePath,
                                            bool doSign,
                                            std::string signerStr)
{
    if (!std::filesystem::exists(tmpFolder))
    {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    std::thread([=]()
                {
        std::unique_ptr<PassHelper> phLocal = std::make_unique<PassHelper>();

        if (!signerStr.empty()) {
            phLocal->setCtxSigners({signerStr});
        }
        std::unique_ptr<InterfacePassFile> pfLocal = phLocal->getPassFile(fullPath);


        return pfLocal->openExternalEncryptWait(encryptTo, watchWaitAndNoneWaitRunCmd,tmpFolder, vscodePath, doSign); })
        .detach();
}

std::string PassFile::openExternalEncryptNoWait(InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder, std::string  vscodePath)
{
    if (!std::filesystem::exists(tmpFolder))
    {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    try
    {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        InterfaceWatchWaitAndNoneWaitRunCmdItem *wi = watchWaitAndNoneWaitRunCmd->addWithOutWait(fullPath, p.filename().u8string(), tmpFolder, vscodePath);
        wi->init();
        PgpmeDataRII din{fullPath, FROM_FILENAME}, dout{wi->getFullFilePath().u8string(), TO_FILENAME};

        g->decryptValidate(din, dout, false);

        return wi->getSubfolderPath().u8string();
    }
    catch (const std::exception &e)
    {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void PassFile::closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
                                          InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,bool doSign)
{
    InterfaceWatchWaitAndNoneWaitRunCmdItem *wi = watchWaitAndNoneWaitRunCmd->getNoneWaitItemsBuUiniqueId(fullPath);

    PgpmeDataRII ein{wi->getFullFilePath().u8string(), FROM_FILENAME};
    PgpmeDataRII eout{fullPath, TO_FILENAME};
    g->encryptSign(ein, eout, encryptTo, doSign);

    watchWaitAndNoneWaitRunCmd->closeWithoutWaitItem(fullPath);
}
