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
            ph = getInterfacePassHelper(true, "",[=](RnpLoginRequestException &e){
                passwordPrompt(e);
                return true; //Rnpgp - Dont abort all multithread, try recover error in specific thread
                            // This introduce extra complexity, espacialy in gui
                            // better return false
            });
            ph->setPasswordCallback([&](std::string keyid) { return getPasswordFromMap(keyid); });
        } else {
            ph = getInterfacePassHelper(false, "",[=](RnpLoginRequestException &e){
                return false; // Gnupg has no multithreading
            });
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


    void  passwordPrompt(RnpLoginRequestException &e)
    {
        
        std::cout << "******** " << e.lastKeyIdRequested << " pass **********\n";
        std::string pass;
        SetStdinEcho(false);
        std::cin >> pass;
        SetStdinEcho(true);
        loginAndPasswordMap[e.lastKeyIdRequested] = pass;
    }

    std::string decryptTestFile(std::string testFile)
    {
        try {
            auto pf = ph->getPassFile(testFile);      
            pf->decrypt();
            return pf->getDecrypted();
        } catch (...) {
            throw;
        }
    }

    void decryptFileToFile(std::string testFile, std::string to)
    {
            auto pf = ph->getPassFile(testFile);
            pf->decryptToFile(to);
    }

    void encryptTextToFile(std::string text,
                           std::string destPath,
                           std::vector<std::string> encryptTo,
                           bool doSign)
    {
            auto pf = ph->getPassFile("");
            pf->encryptStringToFile(text, destPath, encryptTo, doSign);
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
    /*
    for (int i = 0; i < 3; ++i) {
        std::string s = bal.decryptTestFile("/Volumes/RAM_Disk_4G/tmpRepo/template.gpg");
        std::cout << "yes" << "\n";
    }
    */


    bal.searchDown("/Volumes/RAM_Disk_4G/tmpRepo/", ".*.*",
                       ".*a.*"); 

    
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
