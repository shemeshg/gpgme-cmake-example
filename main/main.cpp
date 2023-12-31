#include <iostream>

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
    PassSimpleBal(InterfaceLibgpgfactory *ph)
        : ph{ph}
    {}

    void listKeys(std::string pattern = "", bool secret_only = false)
    {
        for (auto r : ph->listKeys(pattern, secret_only)) {
            std::cout << "we have " << r.getKeyStr() << "\n";
        }
    }

    std::string decryptTestFile(std::string testFile)
    {
        auto pf = ph->getPassFile(testFile);
        pf->decrypt();
        return pf->getDecrypted();
    }

    void decryptFileToFile(std::string testFile, std::string to)
    {
        auto pf = ph->getPassFile(testFile);
        pf->decryptToFile(to);
    }

private:
    InterfaceLibgpgfactory *ph;
};

int main(int, char **)
{
    std::unique_ptr<InterfaceLibgpgfactory> rnpPh = getInterfacePassHelper(true);

    std::unique_ptr<InterfaceLibgpgfactory> gnuPgPh = getInterfacePassHelper(false);
    std::string testFile = "/Volumes/RAM_Disk_4G/tmp/file.gpg";

    rnpPh->setPasswordCallback([&](std::string keyid) {
        std::cout << "******** " << keyid << " pass **********\n";
        std::string pass;
        SetStdinEcho(false);
        std::cin >> pass;
        SetStdinEcho(true);
        return pass;
    });

    PassSimpleBal gnuBal{gnuPgPh.get()};
    PassSimpleBal rnpBal{rnpPh.get()};
    //std::cout << "** from GnuPg \n";
    //gnuBal.listKeys();
    //std::cout<<gnuBal.decryptTestFile(testFile)<<"\n";
    std::cout << "** from RnPgp \n";
    rnpBal.listKeys();
    //RnpLoginRequestException
    try {
        std::cout << rnpBal.decryptTestFile(testFile) << "\n";
        rnpBal.decryptFileToFile(testFile, testFile + ".txt");
    } catch (RnpLoginRequestException &rlre) {
        // Catch the custom exception and print its properties
        std::cout << "Caught RnpLoginRequestException" << std::endl;
        std::cout << "functionName: " << rlre.functionName << std::endl;
        std::cout << "lastKeyIdRequested: " << rlre.lastKeyIdRequested << std::endl;
        std::cout << "filePath: " << rlre.filePath << std::endl;
        throw;
    } catch (std::runtime_error &re) {
        // Catch a std::runtime_error and print its message
        std::cout << "Caught std::runtime_error" << std::endl;
        std::cout << "Message: " << re.what() << std::endl;
        throw;
    } catch (...) {
        // Catch any other exception and print a generic message
        std::cout << "Caught an unknown exception" << std::endl;
        throw;
    }
    return 0;
}
