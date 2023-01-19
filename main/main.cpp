#include <iostream>
// #include "libpgpfactory.h"
#include "libpasshelper.h"
#include "GpgIdManage.h"
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
    GpgIdManage gpgIdManage{};
    gpgIdManage.init("/Users/osx/.password-store/develop/koko/readme.gpg",
                    "/Users/osx/.password-store",&ph);
    //gpgIdManage.saveBackGpgIdFile();
    //gpgIdManage.exportGpgIdToGpgPubKeysFolder();
    //gpgIdManage.importAllGpgPubKeysFolder();
    gpgIdManage.reEncryptStoreFolder();
    for (auto r : ph.listKeys(""))
    {
        std::cout << "we have " << r.getKeyStr() << "\n";
    }    
    /*
    GpgFactory g{};
    g.initPgpFactory();
    g.setArmor(true);
    g.setTextmode(true);
    // g.exportPublicKey("shemeshg","/Volumes/RAM_Disk_4G/gogo");
    // g.importPublicKey("/Volumes/RAM_Disk_4G/keyToImport", true);
    // g.trustPublicKey("0441AFD40FEDEBC0A6C7B5775FA1A6B237A59E25");

    PassHelper ph{};
    for (auto r : g.listKeys(""))
    {
        std::cout << "we have " << r.getKeyStr() << "\n";
    }
    std::cout << ph.getNearestGpgId("/Users/osx/.password-store/develop/koko", "/Users/osx/.password-store") << "\n";
    */
    /*
    PassHelper ph{};

    std::unique_ptr<PassFile> pf = ph.getPassFile("/Users/osx/.password-store/develop/boboadsf.gpg");

    std::cout << "is gpg " << pf->isGpgFile() << " " <<  "\n";
    pf->decrypt();
    std::cout << pf->getDecrypted()<<" \n SIGNED BY \n"
    << pf->getDecryptedSignedBy();
    */

    /*
    std::cout <<ph.getNearestGit("",
                                     "/Users/osx/.password-store")<<"\n";
     */
}
