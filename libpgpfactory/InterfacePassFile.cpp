#include "InterfacePassFile.h"
#include "libpasshelper.h"

#include <numeric>
#include <sstream>
#include <thread>

bool InterfacePassFile::isGpgFile()
{
    std::filesystem::path path(fullPath);
    return (!std::filesystem::is_directory(path) && path.extension().string() == ".gpg");
}

void InterfacePassFile::openExternalEncryptWait(
    std::vector<std::string> encryptTo,
    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
    std::string tmpFolder,
    std::string vscodePath,
    bool doSign)
{
    try {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        InterfaceWatchWaitAndNoneWaitRunCmdItem *wi
            = watchWaitAndNoneWaitRunCmd->addWithWait(fullPath,
                                                      p.filename().u8string(),
                                                      tmpFolder,
                                                      vscodePath);

        wi->init();
        dectyptFileNameToFileName(fullPath, wi->getFullFilePath().u8string());

        wi->runWithWait();
        encryptFileToFile(wi->getFullFilePath().u8string(), fullPath, encryptTo, doSign);
        watchWaitAndNoneWaitRunCmd->runWithWaitClear(*wi);
    } catch (const std::exception &e) {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void InterfacePassFile::openExternalEncryptWaitAsync(
    std::vector<std::string> encryptTo,
    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
    std::string tmpFolder,
    std::string vscodePath,
    bool doSign,
    std::string signerStr)
{
    if (!std::filesystem::exists(tmpFolder)) {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    std::thread([=]() {
        std::unique_ptr<PassHelper> phLocal = std::make_unique<PassHelper>();
        
        if (!signerStr.empty()) {
            phLocal->setCtxSigners({signerStr});
        }
        std::unique_ptr<InterfacePassFile> pfLocal = phLocal->getPassFile(fullPath);
        
        return pfLocal->openExternalEncryptWait(encryptTo,
                                                watchWaitAndNoneWaitRunCmd,
                                                tmpFolder,
                                                vscodePath,
                                                doSign);
    }).detach();
}

std::string InterfacePassFile::openExternalEncryptNoWait(
    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
    std::string tmpFolder,
    std::string vscodePath)
{
    if (!std::filesystem::exists(tmpFolder)) {
        std::throw_with_nested(std::runtime_error("tmp folder not found"));
    }
    try {
        std::filesystem::path p = fullPath;
        p = p.replace_extension();
        InterfaceWatchWaitAndNoneWaitRunCmdItem *wi
            = watchWaitAndNoneWaitRunCmd->addWithOutWait(fullPath,
                                                         p.filename().u8string(),
                                                         tmpFolder,
                                                         vscodePath);
        wi->init();
        dectyptFileNameToFileName(fullPath, wi->getFullFilePath().u8string());

        return wi->getSubfolderPath().u8string();
    } catch (const std::exception &e) {
        watchWaitAndNoneWaitRunCmd->clearWaitItemsAfterUnExpectedCrash(fullPath);
        throw;
    }
}

void InterfacePassFile::closeExternalEncryptNoWait(
    std::vector<std::string> encryptTo,
    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
    bool doSign)
{
    InterfaceWatchWaitAndNoneWaitRunCmdItem *wi
        = watchWaitAndNoneWaitRunCmd->getNoneWaitItemsBuUiniqueId(fullPath);

    encryptFileToFile(wi->getFullFilePath().u8string(), fullPath, encryptTo, doSign);

    watchWaitAndNoneWaitRunCmd->closeWithoutWaitItem(fullPath);
}

void InterfacePassFile::setFullPath(std::string s)
{
    fullPath = s;
    decryptedSignedBy = {};
}

std::string InterfacePassFile::getDecryptedSignedBy()
{
    std::ostringstream imploded;
    auto vec = getPubIdDecryptedSignedBy();
    return decryptedSignedBy.empty() ? "" : /* leave early if there are no items in the list */
               std::accumulate(             /* otherwise, accumulate */
                               ++vec.begin(),
                               vec.end(),    /* the range 2nd to after-last */
                               *vec.begin(), /* and start accumulating with the first item */
                               [](auto &a, auto &b) { return a + "," + b; });
}