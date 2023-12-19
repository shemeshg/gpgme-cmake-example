#include "libpasshelper.h"
#include <filesystem>
#include "PassFile.h"

std::vector<GpgKeys> PassHelper::listKeys(const std::string pattern, bool secret_only) {
    return GpgFactory::listKeys(pattern,secret_only);
}

void PassHelper::setCtxSigners(std::vector<std::string> signedBy) {
    return GpgFactory::setCtxSigners(signedBy);
}

void PassHelper::exportPublicKey(const std::string &keyId, const std::string &filePath)
{
    return GpgFactory::exportPublicKey(keyId, filePath);
}

void PassHelper::importPublicKey(const std::string &filePath, bool doTrust)
{
    return GpgFactory::importPublicKey(filePath,doTrust);
}

void PassHelper::trustPublicKey(const std::string &keyId)
{
    return GpgFactory::trustPublicKey(keyId);
}

std::unique_ptr<InterfacePassFile> PassHelper::getPassFile(std::string fullPath)
{
    return std::make_unique<PassFile>(fullPath, this);
}

bool PassHelper::isGpgFile(std::string pathToFile){
    std::unique_ptr<InterfacePassFile> pf = getPassFile(pathToFile);
    return  pf->isGpgFile();
}

std::string PassHelper::getNearestGpgId(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
}

void PassHelper::encryptFolderToFolder(std::string folderFrom, std::string folderTo, std::vector<std::string> encryptTo, bool doSign)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom, ".*.*", ".*.*",
        [&](std::string path)
        {
            return true;
        },
        [&](std::string path)
        {
            std::filesystem::path toPath = std::filesystem::path(folderTo) /
                                           std::filesystem::relative(path, folderFrom);

            std::filesystem::create_directories(toPath.parent_path());
            pf->encryptFileToFile(path, toPath.generic_string() + ".gpg",encryptTo, doSign);

            return true;
        });
}

void PassHelper::decryptFolderToFolder(std::string folderFrom, std::string folderTo)
{
    std::unique_ptr<InterfacePassFile> pf = getPassFile("");
    fileSearch.searchDown(
        folderFrom, ".*.*", ".*.*",
        [&](std::string path)
        {
            if (!isGpgFile(path)){
                //std::cout << path << " \n Skiped Not a .gpg file\n";
                return false;
            }
            return true;
        },
        [&](std::string path)
        {
            std::filesystem::path toPath = std::filesystem::path(folderTo) /
                                           std::filesystem::relative(path, folderFrom);

            std::filesystem::create_directories(toPath.parent_path());
            pf->setFullPath(path);
            pf->decryptToFile(toPath.replace_extension().u8string());
            //std::cout << path << " \n to" << toPath.parent_path() << "\n";
            return true;
        });
}

std::string PassHelper::getNearestTemplateGpg(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp("template.gpg", currentPath, stopPath);
}

std::string PassHelper::getNearestGit(std::string currentPath, std::string stopPath)
{
    return fileSearch.searchUp(".git", currentPath, stopPath);
}

void PassHelper::reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo,
                               bool doSign                      )
{
    try {
        std::string backupFile = pathFileToReEncrypt + "backup";
        std::filesystem::rename(pathFileToReEncrypt,backupFile);
        PgpmeDataRII ein{backupFile, FROM_FILENAME},
            emem{},
            eout{pathFileToReEncrypt, TO_FILENAME};
        decryptValidate(ein, emem, false);
        emem.getString(); //get fseek to end of buffer
        encryptSign(emem, eout, encryptTo, doSign);
        std::filesystem::remove(backupFile);
    }
    catch (...){
        std::filesystem::rename(pathFileToReEncrypt + "backup", pathFileToReEncrypt);
        throw;
    }
}

void PassHelper::reEncryptStoreFolder(std::string nearestGpgIdFolder, std::vector<std::string> encryptTo, std::function<void (std::string)> func, bool doSign){
    fileSearch.searchDown(
        nearestGpgIdFolder, ".*.*", ".*.*",
        [&](std::string path)
        {
            if (!isGpgFile(path)){
                //std::cout << path << " \n Skiped Not a .gpg file\n";
                return false;
            }
            return true;
        },
        [&](std::string path)
        {
            func(path);
            //std::cout << "Re-encrypt " << path<< "\n";
            reEncryptFile(path, encryptTo, doSign);
            //std::cout << " Finished\n";
            return true;
        });
}
void PassHelper::searchDown(std::string FolderToSearch,
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
