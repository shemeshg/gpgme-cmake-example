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
    //setupToEmpty() setupToFile()  #to_class, setupFromString("string"), setupFromFile() # from_class
    //exportString(), exportConsole(), exportFile() 
    //encrypt without sign
    //gpgme_data_new_from_file
    std::string out = g.encryptSign("shalom olam",{"shemeshg"});
    std::cout<<"Finished\n"<<out<<"\n";
    
}
