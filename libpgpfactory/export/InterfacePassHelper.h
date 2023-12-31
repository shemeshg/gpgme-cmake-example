#pragma once
#include "FileSearch.h"
#include "GpgKeys.h"
#include "InterfacePassFile.h"
#include <map>
#include <regex>

class InterfaceLibgpgfactory
{
public:
    virtual std::vector<GpgKeys> listKeys(const std::string pattern = "", bool secret_only = false)
        = 0;
    virtual void setCtxSigners(std::vector<std::string> signedBy) = 0;

    virtual void exportPublicKey(std::string const &keyId, std::string const &filePath) = 0;
    virtual void importPublicKey(std::string const &filePath, bool doTrust) = 0;
    virtual void trustPublicKey(std::string const &keyId) = 0;

    virtual ~InterfaceLibgpgfactory() {}

    virtual std::unique_ptr<InterfacePassFile> getPassFile(std::string fullPath) = 0;
    bool isGpgFile(std::string pathToFile)
    {
        std::unique_ptr<InterfacePassFile> pf = getPassFile(pathToFile);
        return pf->isGpgFile();
    }

    virtual std::string getNearestGit(std::string currentPath, std::string stopPath)
    {
        return fileSearch.searchUp(".git", currentPath, stopPath);
    }

    virtual std::string getNearestGpgId(std::string currentPath, std::string stopPath)
    {
        return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
    }

    virtual std::string getNearestTemplateGpg(std::string currentPath, std::string stopPath)
    {
        return fileSearch.searchUp("template.gpg", currentPath, stopPath);
    }

    virtual void encryptFolderToFolder(std::string folderFrom,
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

    virtual void decryptFolderToFolder(std::string folderFrom, std::string folderTo)
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

    virtual void searchDown(std::string FolderToSearch,
                            std::string fileRegExStr,
                            std::string contentRegExStr,
                            const std::vector<std::string> &ignoreBinaryExtensions,
                            bool isMemCash,
                            std::map<std::string, std::string> &searchMemCash,
                            std::function<void(std::string s)> callback)
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

    virtual void reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign)
    {
        std::unique_ptr<InterfacePassFile> pf = getPassFile("");
        return pf->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);
    }

    virtual void reEncryptStoreFolder(std::string nearestGpgIdFolder,
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

private:
    FileSearch fileSearch{};
};

std::unique_ptr<InterfaceLibgpgfactory> getInterfacePassHelper();
