#include <iostream>
#include "libpgpfactory.h"





int main(int, char **)
{
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

    PgpmeDataRII din{"/Volumes/RAM_Disk_4G/tmp/gogo.gpg",FROM_FILENAME}, dout{};
    g.decryptValidate(din,dout,true);
    dout.PrintStdout();
}
