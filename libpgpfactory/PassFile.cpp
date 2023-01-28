#include "PassFile.h"
#include "WatchWaitAndNoneWaitRunCmdItem.h"

PassFile::PassFile(std::string fullPath, GpgFactory *g) : fullPath{fullPath}, g{g}
{
}

bool PassFile::isGpgFile()
{
    std::filesystem::path path(fullPath);
    return (path.extension().string() == ".gpg");
}

void PassFile::decrypt()
{
    PgpmeDataRII din{fullPath, FROM_FILENAME},
        dout{};
    g->decryptValidate(din, dout, false);
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

    return decryptedSignedBy.empty() ? "" :                                          /* leave early if there are no items in the list */
               std::accumulate(                                                      /* otherwise, accumulate */
                               ++decryptedSignedBy.begin(), decryptedSignedBy.end(), /* the range 2nd to after-last */
                               *decryptedSignedBy.begin(),                           /* and start accumulating with the first item */
                               [](auto &a, auto &b)
                               { return a + "," + b; });
}

void PassFile::encrypt(std::string s, std::vector<std::string> encryptTo)
{
    decrypted = s;
    PgpmeDataRII din{s, FROM_STRING}, dout{fullPath, TO_FILENAME};
    g->encryptSign(din, dout, encryptTo, true);
}

void PassFile::openExternalEncryptWait(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder)
{
    try
    {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        WatchWaitAndNoneWaitRunCmdItem wi = watchWaitAndNoneWaitRunCmd->addWithWait(fullPath, p.filename(), tmpFolder);

        wi.init();
        PgpmeDataRII din{fullPath, FROM_FILENAME}, dout{wi.getFullFilePath(), TO_FILENAME};

        g->decryptValidate(din, dout, false);
        din.closeFiles();
        dout.closeFiles();

        watchWaitAndNoneWaitRunCmd->runWithWait(wi);
        PgpmeDataRII ein{wi.getFullFilePath(), FROM_FILENAME};
        PgpmeDataRII eout{fullPath, TO_FILENAME};
        g->encryptSign(ein, eout, encryptTo, true);
    }
    catch (const std::exception &e)
    {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void PassFile::openExternalEncryptWaitAsync(std::vector<std::string> encryptTo, WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder)
{
    if (!std::filesystem::exists(tmpFolder))
    {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    std::thread([=]()
                {
        PassFile threadassfile{fullPath,g};
        return threadassfile.openExternalEncryptWait(encryptTo, watchWaitAndNoneWaitRunCmd,tmpFolder); })
        .detach();
}

std::string PassFile::openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd, std::string tmpFolder)
{
    if (!std::filesystem::exists(tmpFolder))
    {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    try
    {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        WatchWaitAndNoneWaitRunCmdItem *wi = watchWaitAndNoneWaitRunCmd->addWithOutWait(fullPath, p.filename(), tmpFolder);
        wi->init();
        PgpmeDataRII din{fullPath, FROM_FILENAME}, dout{wi->getFullFilePath(), TO_FILENAME};

        g->decryptValidate(din, dout, false);
        din.closeFiles();
        dout.closeFiles();

        return wi->getSubfolderPath();
    }
    catch (const std::exception &e)
    {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void PassFile::closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
                                          WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd)
{
    WatchWaitAndNoneWaitRunCmdItem *wi = watchWaitAndNoneWaitRunCmd->getNoneWaitItemsBuUiniqueId(fullPath);

    PgpmeDataRII ein{wi->getFullFilePath(), FROM_FILENAME};
    PgpmeDataRII eout{fullPath, TO_FILENAME};
    g->encryptSign(ein, eout, encryptTo, true);
    watchWaitAndNoneWaitRunCmd->closeWithoutWaitItem(fullPath);
}
