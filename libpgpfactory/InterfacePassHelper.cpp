#include "InterfacePassHelper.h"
#include "RnpHelper.h"
#include "libpasshelper.h"
#include <regex>

std::unique_ptr<InterfaceLibgpgfactory> getInterfacePassHelper(bool isRnPgp)
{
    if (isRnPgp) {
        std::unique_ptr<InterfaceLibgpgfactory> rnpHelper = std::make_unique<RnpHelper>();
        return rnpHelper;
    }
    std::unique_ptr<InterfaceLibgpgfactory> passHelper = std::make_unique<PassHelper>();
    return passHelper;
}
bool InterfaceLibgpgfactory::isGpgFile(std::string pathToFile)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile(pathToFile);
    return pf->isGpgFile();
}

std::string InterfaceLibgpgfactory::getNearestGit(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp(".git", currentPath, stopPath);
}

void InterfaceLibgpgfactory::encryptFolderToFolder(std::string folderFrom,
                                                   std::string folderTo,
                                                   std::vector<std::string> encryptTo,
                                                   bool doSign)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom,
        ".*.*",
        ".*.*",
        [&](std::string path) { return true; },
        [&](std::string path) {
            std::filesystem::path toPath = std::filesystem::path(folderTo)
                                           / std::filesystem::relative(path, folderFrom);

            std::filesystem::create_directories(toPath.parent_path());
            pf->encryptFileToFile(path, toPath.generic_string() + ".gpg", encryptTo, doSign);

            return true;
        });
}

std::string InterfaceLibgpgfactory::getNearestTemplateGpg(std::string currentPath,
                                                          std::string stopPath)
{
    return fileSearch.searchUp("template.gpg", currentPath, stopPath);
}

void InterfaceLibgpgfactory::decryptFolderToFolder(std::string folderFrom, std::string folderTo)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom,
        ".*.*",
        ".*.*",
        [&](std::string path) {
            if (!isGpgFile(path)) {
                //std::cout << path << " \n Skiped Not a .gpg file\n";
                return false;
            }
            return true;
        },
        [&](std::string path) {
            std::filesystem::path toPath = std::filesystem::path(folderTo)
                                           / std::filesystem::relative(path, folderFrom);

            std::filesystem::create_directories(toPath.parent_path());
            pf->setFullPath(path);
            pf->decryptToFile(toPath.replace_extension().u8string());
            //std::cout << path << " \n to" << toPath.parent_path() << "\n";
            return true;
        });
}

void InterfaceLibgpgfactory::reEncryptFile(std::string pathFileToReEncrypt,
                                           std::vector<std::string> encryptTo,
                                           bool doSign)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    return pf->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);
}

void InterfaceLibgpgfactory::reEncryptStoreFolder(std::string nearestGpgIdFolder,
                                                  std::vector<std::string> encryptTo,
                                                  std::function<void(std::string)> func,
                                                  bool doSign)
{
    fileSearch.searchDown(
        nearestGpgIdFolder,
        ".*.*",
        ".*.*",
        [&](std::string path) {
            if (!isGpgFile(path)) {
                //std::cout << path << " \n Skiped Not a .gpg file\n";
                return false;
            }
            return true;
        },
        [&](std::string path) {
            func(path);
            //std::cout << "Re-encrypt " << path<< "\n";
            reEncryptFile(path, encryptTo, doSign);
            //std::cout << " Finished\n";
            return true;
        });
}

void InterfaceLibgpgfactory::searchDown(std::string FolderToSearch,
                                        std::string fileRegExStr,
                                        std::string contentRegExStr,
                                        const std::vector<std::string> &ignoreBinaryExtensions,
                                        bool isMemCash,
                                        std::map<std::string, std::string> &searchMemCash,
                                        std::function<void(std::string)> callback)
{
    if (!isMemCash) {
        searchMemCash.clear();
    }
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    fileSearch.searchDown(
        FolderToSearch,
        fileRegExStr,
        contentRegExStr,
        [&](std::string path) {
            if (contentRegExStr == ".*.*") {
                return true;
            }

            std::filesystem::path filePath(path);

            if (std::find(ignoreBinaryExtensions.begin(),
                          ignoreBinaryExtensions.end(),
                          filePath.stem().extension())
                != ignoreBinaryExtensions.end()) {
                return false;
            }

            std::string content;
            bool foundInCash = false;
            long int millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::filesystem::last_write_time(filePath).time_since_epoch())
                                  .count();

            std::string memCashKey = std::to_string(millis) + path;
            if (isMemCash) {
                if (searchMemCash.count(memCashKey) > 0) {
                    content = searchMemCash[memCashKey];
                    foundInCash = true;
                }
            }

            if (!foundInCash) {
                pf->setFullPath(path);

                if (!pf->isGpgFile()) {
                    return false;
                }
                try {
                    pf->decrypt();

                    content = pf->getDecrypted();
                    if (isMemCash) {
                        searchMemCash[memCashKey] = content;
                    }
                } catch (const std::exception &e) {
                    throw std::runtime_error(path + ": " + e.what());
                }
            }

            const std::regex contentRegEx(contentRegExStr, std::regex_constants::icase);

            content = std::regex_replace(content, std::regex("\\r\\n|\\r|\\n"), "");

            bool a = std::regex_match(content, contentRegEx);

            return a;
        },
        callback);
}

std::string InterfaceLibgpgfactory::getNearestGpgId(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
}
