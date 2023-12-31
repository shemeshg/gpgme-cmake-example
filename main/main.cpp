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
            ph = getInterfacePassHelper(true);
            ph->setPasswordCallback([&](std::string keyid) { return getPasswordFromMap(keyid); });
        } else {
            ph = getInterfacePassHelper(false);
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
        } else if ( e.functionName == "decryptFileToFile"){
            decryptFileToFile(e.fromFilePath,e.toFilePath);
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

private:
    std::unique_ptr<InterfaceLibgpgfactory> ph;
    std::map<std::string, std::string> loginAndPasswordMap{};
};

int main(int, char **)
{
    std::string testFile = "/Volumes/RAM_Disk_4G/tmp/file.gpg";

    PassSimpleBal gnuBal{false};
    PassSimpleBal rnpBal{true};
    //std::cout << "** from GnuPg \n";
    //gnuBal.listKeys();
    //std::cout<<gnuBal.decryptTestFile(testFile)<<"\n";
    std::cout << "** from RnPgp \n";
    rnpBal.listKeys();
    //RnpLoginRequestException

    //std::cout << rnpBal.decryptTestFile(testFile) << "\n";
    rnpBal.decryptFileToFile(testFile, testFile + ".txt");

    return 0;
}
