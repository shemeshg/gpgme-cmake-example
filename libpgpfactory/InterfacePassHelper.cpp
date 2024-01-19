#include "InterfacePassHelper.h"
#include "RnpHelper.h"
#include "RnpLoginRequestException.h"
#if defined(__APPLE__) || defined(__linux__)
#include "libpasshelper.h"
#endif
#include <regex>

class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    static Singleton &getInstance()
    {
        // Static local variable that holds the singleton object
        static Singleton instance;
        return instance;
    }

    InterfaceLibgpgfactory *getHelper(bool isRnPgp, std::string rnpHomePath)
    {
        if (helper == nullptr) {
#if defined(__APPLE__) || defined(__linux__)
            if (isRnPgp) {
                helper = std::make_unique<RnpHelper>(rnpHomePath);
            } else {
                helper = std::make_unique<PassHelper>();
            }
#else
            helper = std::make_unique<RnpHelper>(rnpHomePath);
#endif
        }
        return helper.get();
    }

private:
    // Private constructor
    std::unique_ptr<InterfaceLibgpgfactory> helper = nullptr;
    Singleton() {}

    // Private destructor
    ~Singleton() {}
};

InterfaceLibgpgfactory *getInterfacePassHelper(bool isRnPgp, std::string rnpHomePath)
{
    Singleton &s = Singleton::getInstance();
    return s.getHelper(isRnPgp, rnpHomePath);
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
        },
        useMultiThread());
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
        },useMultiThread());
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
        },useMultiThread() );
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
                } catch (RnpLoginRequestException &rlre) {
                    throw;
                } catch (const std::exception &e) {
                    throw std::runtime_error(path + ": " + e.what());
                } catch (...) {
                    throw;
                }
            }

            std::regex regIsSimpleFindSearch("^\\.\\*[^\\.\\*]*\\.\\*$");
            if (regex_match(contentRegExStr, regIsSimpleFindSearch)) {
                std::regex matchExpresion("\\.\\*(.*)\\.\\*");
                std::smatch m;
                regex_search(contentRegExStr, m, matchExpresion);
                std::string searchStr = m[1];
                return (content.find(searchStr) != std::string::npos);
            } else {
                const std::regex contentRegEx(contentRegExStr, std::regex_constants::icase);
                content = std::regex_replace(content, std::regex("\\r\\n|\\r|\\n"), "");
                return std::regex_match(content, contentRegEx);
            }
        },
        callback,useMultiThread() );
}

std::string InterfaceLibgpgfactory::getNearestGpgId(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
}
