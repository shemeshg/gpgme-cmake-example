#include <iostream>

#include "libpasshelper.h"
#include "GpgIdManage.h"
#include "RunShellCmd.h"
#include "FileSearch.h"

class PassSimpleBal {
public:
    PassHelper ph{};
    void listKeys(std::string pattern="", bool secret_only=false){
        for (auto r : ph.listKeys(pattern, secret_only))
        {
            std::cout << "we have " << r.getKeyStr() << "\n";
        }  
    }
};



int main(int, char **)
{   
    PassSimpleBal p{};
    p.listKeys();
    
   return 0;
}
