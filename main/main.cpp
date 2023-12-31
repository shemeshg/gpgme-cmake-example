#include <iostream>

//#include "libpasshelper.h"
//#include "GpgIdManage.h"
//#include "RunShellCmd.h"
//#include "FileSearch.h"
#include "InterfacePassHelper.h"

#include "RnpCoreInterface.h"
#include "RnpKeys.h"
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

private:
    InterfaceLibgpgfactory *ph;
};

int main(int, char **)
{
    std::unique_ptr<InterfaceLibgpgfactory> gnuPgPh = getInterfacePassHelper(false);
    std::unique_ptr<InterfaceLibgpgfactory> rnpPh = getInterfacePassHelper(true);
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
    std::cout << "** from GnuPg \n";
    //gnuBal.listKeys();
    std::cout<<gnuBal.decryptTestFile(testFile)<<"\n";
    std::cout << "** from RnPgp \n";
    //rnpBal.listKeys();
     std::cout<<rnpBal.decryptTestFile(testFile)<<"\n";

    return 0;
}
