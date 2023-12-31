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

private:
    InterfaceLibgpgfactory *ph;
};

int main(int, char **)
{
    std::unique_ptr<InterfaceLibgpgfactory> gnuPgPh = getInterfacePassHelper(false);
    std::unique_ptr<InterfaceLibgpgfactory> rnpPh = getInterfacePassHelper(true);
    std::cout << "** from GnuPg \n";
    PassSimpleBal gnuBal{gnuPgPh.get()};
    PassSimpleBal rnpBal{rnpPh.get()};
    gnuBal.listKeys();
    std::cout << "** from RnPgp \n";
    rnpBal.listKeys();
    /*
    std::unique_ptr<RnpCoreInterface> rbl = getRnpCoreInterface();

    rbl->setPasswordCallback([&](std::string keyid) {
        std::cout << "******** " << keyid << " pass **********\n";
        std::string pass;
        SetStdinEcho(false);
        std::cin >> pass;
        SetStdinEcho(true);
        return pass;
    });


    std::cout << "RNP version: " << rbl->getRnpVersionString() << "\n";

    for (auto &k : rbl->listKeys("", false)) {
        std::cout << k.getKeyStr() << "\n";
    }
    */
    return 0;
}
