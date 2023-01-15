#include <iostream>
// #include "libpgpfactory.h"
#include "libpasshelper.h"

/*
void testLibrary(){
    GpgFactory g{};
    g.initPgpFactory();
    g.setArmor(true);
    //g.setTextmode(true);
    for(auto r : g.listKeys()){
        std::cout<<"we have "<<r.getKeyStr()<<"\n";
    }
    //g.getGpgMeKeys({"shemeshg"});
    g.setCtxSigners({"shemeshg"});

    //PgpmeDataRII ein{"/Volumes/RAM_Disk_4G/tmp/bb.txt",FROM_FILENAME},
    //            eout{"/Volumes/RAM_Disk_4G/tmp/gogo.gpg",TO_FILENAME};

    //g.encryptSign(ein, eout ,{"shemeshg"}, false);
    //std::cout<<"Finished\n"<<out.getString()<<"\n";
    //eout.PrintStdout();
    //out.writeToFile("/Volumes/RAM_Disk_4G/tmp/writeText.pgp");

    PgpmeDataRII din{"/Volumes/RAM_Disk_4G/tmp/gogo.gpg",FROM_FILENAME},
                    dout{"/Volumes/RAM_Disk_4G/tmp/world.txt",TO_FILENAME};
    g.decryptValidate(din,dout,false);
    dout.PrintStdout();
}
*/

int main(int, char **)
{
    PassHelper ph{};
    /*
    std::unique_ptr<PassFile> pf = ph.getPassFile("/Users/osx/.password-store/develop/boboadsf.gpg");
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "is gpg " << pf->isGpgFile() << " " << i << "\n";
        pf->decrypt();
        // std::cout << pf->getDecrypted();
        pf->getDecrypted();
    }
    */
   std::cout <<ph.getNearestGpgId("/Users/osx/.password-store/develop/koko/hu",
                                    "/Users/osx/.password-store")<<"\n";
}