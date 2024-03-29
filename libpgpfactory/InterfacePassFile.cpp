#include "InterfacePassFile.h"
#include "InterfacePassHelper.h"
#include "uuid.h"

#include <numeric>
#include <sstream>
#include <thread>

bool InterfacePassFile::isGpgFile()
{
    std::filesystem::path path(fullPath);
    return (!std::filesystem::is_directory(path) && path.extension().string() == ".gpg");
}

const std::string InterfacePassFile::getFullPathFolder()
{
    if (std::filesystem::is_directory(fullPath)) {
        return fullPath;
    }
    std::filesystem::path f{fullPath};
    return f.parent_path().u8string();
}

void InterfacePassFile::encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign)
{
    std::string tmpName = fullPath + uuid::generate_uuid_v4();

    decrypted = s;
    try {
        encryptStringToFile(s, tmpName, encryptTo, doSign);
    } catch (...) {
        std::filesystem::remove(tmpName);
        throw;
    }

    std::filesystem::rename(tmpName, fullPath);
}

InterfaceWatchWaitAndNoneWaitRunCmdItem *InterfacePassFile::openExternalEncryptNoWait(
    InterfaceWatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
    std::string tmpFolder,
    std::string vscodePath,
    RunShellCmd *rsc)
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
                                                         vscodePath,
                                                         rsc);
        wi->init();
        dectyptFileNameToFileName(fullPath, wi->getFullFilePath().u8string());

        return wi;
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
