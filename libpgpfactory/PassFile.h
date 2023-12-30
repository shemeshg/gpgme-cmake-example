#pragma once
#include "InterfacePassFile.h"
#include "InterfaceWatchWaitAndNoneWaitRunCmd.h"
#include "libpgpfactory.h"
#include <numeric>
#include <string>
#include <thread>
#include <vector>

class PassFile : public InterfacePassFile
{
public:
    PassFile(std::string fullPath, GpgFactory *g);

    void decrypt() override;

    void encrypt(std::string s, std::vector<std::string> encryptTo, bool doSign) override;

    void encryptStringToFile(std::string s,
                             std::string toFileName,
                             std::vector<std::string> encryptTo,
                             bool doSign) override;
    void encryptFileToFile(std::string fromFileName,
                           std::string toFileName,
                           std::vector<std::string> encryptTo,
                           bool doSign) override;
    void decryptToFile(std::string toFileName) override;

    void dectyptFileNameToFileName(std::string fromPath, std::string toPath) override;

private:
    GpgFactory *g;

    std::vector<std::string> getPubIdDecryptedSignedBy() override
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
