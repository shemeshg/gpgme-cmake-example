#include <iostream>
#include <map>

//#include "libpasshelper.h"
//#include "GpgIdManage.h"
//#include "RunShellCmd.h"
//#include "FileSearch.h"
#include "InterfacePassHelper.h"

#include "RnpCoreInterface.h"
#include "RnpKeys.h"
#include "RnpLoginRequestException.h"
#include "SetStdinEcho.h"

class PassSimpleBal
{
public:
    PassSimpleBal(bool isRnPgp)
    {
        if (isRnPgp) {
            ph = getInterfacePassHelper(true, "");
            ph->setPasswordCallback([&](std::string keyid) { return getPasswordFromMap(keyid); });
        } else {
            ph = getInterfacePassHelper(false, "");
        }
    }

    std::string promptForPasswordAndRetry(RnpLoginRequestException &e)
    {
        std::cout << "******** " << e.lastKeyIdRequested << " pass **********\n";
        std::string pass;
        SetStdinEcho(false);
        std::cin >> pass;
        SetStdinEcho(true);
        loginAndPasswordMap[e.lastKeyIdRequested] = pass;

        if (e.functionName == "decryptFileToString") {
            return decryptTestFile(e.fromFilePath);
        } else if (e.functionName == "decryptFileToFile") {
            decryptFileToFile(e.fromFilePath, e.toFilePath);
            return "";
        } else if (e.functionName == "encryptSignStringToFile") {
            encryptTextToFile(e.fromFilePath, e.toFilePath, e.encryptTo, e.doSign);
            return "";
        } else {
            std::cout << "unknown function type " << e.functionName << "\n";
            return "";
        }
    }

    std::string getPasswordFromMap(std::string keyid)
    {
        std::string pass = "";
        if (loginAndPasswordMap.count(keyid)) {
            pass = loginAndPasswordMap.at(keyid);
        }
        return pass;
    }

    void listKeys(std::string pattern = "", bool secret_only = false)
    {
        for (auto r : ph->listKeys(pattern, secret_only)) {
            std::cout << "we have " << r.getKeyStr() << "\n";
        }
    }

    std::string decryptTestFile(std::string testFile)
    {
        try {
            auto pf = ph->getPassFile(testFile);
            pf->decrypt();
            return pf->getDecrypted();
        } catch (RnpLoginRequestException &rlre) {
            return promptForPasswordAndRetry(rlre);
        } catch (...) {
            throw;
        }
    }

    void decryptFileToFile(std::string testFile, std::string to)
    {
        try {
            auto pf = ph->getPassFile(testFile);
            pf->decryptToFile(to);
        } catch (RnpLoginRequestException &rlre) {
            promptForPasswordAndRetry(rlre);
        } catch (...) {
            throw;
        }
    }

    void encryptTextToFile(std::string text,
                           std::string destPath,
                           std::vector<std::string> encryptTo,
                           bool doSign)
    {
        try {
            auto pf = ph->getPassFile("");
            pf->encryptStringToFile(text, destPath, encryptTo, doSign);
        } catch (RnpLoginRequestException &rlre) {
            promptForPasswordAndRetry(rlre);
        } catch (...) {
            throw;
        }
    }

    void setSigners(std::vector<std::string> s) { ph->setCtxSigners(s); }

    void searchDown(std::string FolderToSearch,
                    std::string fileRegExStr,
                    std::string contentRegExStr)
    {
        std::map<std::string, std::string> emptyMap;

        ph->searchDown(FolderToSearch,
                       fileRegExStr,
                       contentRegExStr,
                       true,
                       {},
                       {},
                       false,
                       emptyMap,
                       [&](std::string path) {
                           std::cout << "found in " << path << " mth " << ph->useMultiThread()
                                     << "\n";
                           return true;
                       });
    }

private:
    InterfaceLibgpgfactory *ph;
    std::map<std::string, std::string> loginAndPasswordMap{};
};

int main(int, char **)
{
    std::string testFile = "/Volumes/RAM_Disk_4G/tmp/file.gpg";

    PassSimpleBal bal{true};

    bal.listKeys();
    for (int i = 0; i < 10; ++i) {
        std::string s = bal.decryptTestFile("/Volumes/RAM_Disk_4G/tmpRepo/template.gpg");
        std::cout << "yes" << "\n";
    }
    for (int i = 0; i < 10; ++i) {
        bal.searchDown("/Volumes/RAM_Disk_4G/tmpRepo/", ".*.*",
                       ".*a.*"); 
    }
    /*
   
    bal.setSigners({"1CA9424DDD85177F"});
    std::cout << "** from GnuPg \n";
    
    //std::cout<<bal.decryptTestFile(testFile)<<"\n";

    bal.encryptTextToFile("shalom\nolam\n",
                          "/Volumes/RAM_Disk_4G/tmp/shalom.gpg",
                          {"1CA9424DDD85177F"},
                          true);
   */
    return 0;
}
