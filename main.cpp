#include <iostream>
#include "libpgpfactory.h"





int main(int, char **)
{
    GpgFactory g{};
    g.initPgpFactory();
    for(auto r : g.listKeys()){
        std::cout<<"we have "<<r.getKeyStr()<<"\n";
    }

    
}
