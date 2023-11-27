#pragma once
#include "WatchWaitAndNoneWaitRunCmd.h"
#include "libpgpfactory.h"
#include <numeric>
#include <string>
#include <thread>
#include <vector>

class PassFile
{
public:
    PassFile(std::string fullPath, GpgFactory *g);

    bool isGpgFile();

    void decrypt();

    std::string &getDecrypted();

    std::string const &getFullPath() { return fullPath; }

    std::string const getFullPathFolder()
    {
        if (std::filesystem::is_directory(fullPath)) {
            return fullPath;
        }
        std::filesystem::path f{fullPath};
        return f.parent_path();
    }

    std::string getDecryptedSignedBy();

    void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign);

    void openExternalEncryptWait(std::vector<std::string> encryptTo,
                                 WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                 std::string tmpFolder,
                                 std::string vscodePath,
                                 bool doSign);
    void openExternalEncryptWaitAsync(std::vector<std::string> encryptTo,
                                      WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                      std::string tmpFolder,
                                      std::string vscodePath,
                                      bool doSign);

    std::string openExternalEncryptNoWait(WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                          std::string tmpFolder,
                                          std::string vscodePath);
    void closeExternalEncryptNoWait(std::vector<std::string> encryptTo,
                                    WatchWaitAndNoneWaitRunCmd *watchWaitAndNoneWaitRunCmd,
                                    bool doSign);

    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign);
    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign);
    void decryptToFile(std::string toFileName);

    void dectyptFileNameToFileName(std::string fromPath, std::string toPath);

    void setFullPath(std::string s) { fullPath = s; decryptedSignedBy = {};}

private:
    std::string fullPath, decrypted;
    std::vector<std::string> decryptedSignedBy = {};
    GpgFactory *g;

    std::vector<std::string> getPubIdDecryptedSignedBy()
    {
        std::vector<std::string> ret;
        for (const auto &row : decryptedSignedBy) {
            auto svec = g->listKeys(row);
            if (svec.size() == 1) {
                ret.push_back(svec[0].getKeyStr());
            } else {
                ret.push_back(row);
            }
        }
        return ret;
    };
};
