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

std::vector<std::string> split(std::string s)
{
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}

// #include <fstream>
void validateNearestGpgId()
{
    GpgFactory g{};
    g.initPgpFactory();
    g.setArmor(true);
    g.setTextmode(true);
    PassHelper ph{};

    // Return class vars
    std::vector<GpgKeys> allKeys = g.listKeys("");    
    std::string nearestGpgId = ph.getNearestGpgId("/Users/osx/.password-store/develop/koko", "/Users/osx/.password-store");
    std::string gpgPubKeysFolder = nearestGpgId + "/.gpg-pub-keys";
    bool gpgPubKeysFolderExists = false;

    std::ifstream infile(nearestGpgId + "/.gpg-id");
    std::vector<std::string> stringsNotFound{};
    std::vector<GpgKeys> keysFound{};

    // End Return class vars
    gpgPubKeysFolderExists = checkGpgPubKeysFolderExists

    std::string line;
    while (std::getline(infile, line))
    {
        auto keyToSearchVector = split(line);
        if (keyToSearchVector.size() == 0)
        {
            continue;
        }

        std::string &keyToSearch = keyToSearchVector.at(0);
        std::cout << "Searching " << keyToSearch << "\n";
        std::vector<GpgKeys> keysListed = g.listKeys(keyToSearch);
        if (keysListed.size() == 1)
        {
            keysFound.push_back(std::move(keysListed.at(0)));
        }
        else
        {
            stringsNotFound.push_back(keyToSearch);
        }
    }
    std::cout << "Finished\n";
}

int main(int, char **)
{
    validateNearestGpgId();
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
