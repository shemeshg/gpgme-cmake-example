#include "GpgIdManage.h"

void GpgIdManage::init(std::string _currentPath, std::string _stopPath, PassHelper *_ph)
{
    currentPath = _currentPath;
    stopPath = _stopPath;
    ph = _ph;

    keysFoundInGpgIdFile.clear();
    KeysNotFoundInGpgIdFile.clear();
    allKeys.clear();
    encryptTo.clear();


    allKeys = ph->listKeys("");
    nearestGpgIdFolder = ph->getNearestGpgId(currentPath, stopPath);
    gpgPubKeysFolder = nearestGpgIdFolder + "/.gpg-pub-keys";
    nearestGpgIdFile = nearestGpgIdFolder + "/.gpg-id";
    gpgPubKeysFolderExists = std::filesystem::exists(gpgPubKeysFolder);
    populateKeysParsedInGpgIdFile();

    encryptTo = {};
    for (auto r : keysFoundInGpgIdFile)
    {
        encryptTo.push_back(r.keyid);
    }

    classInitialized = true;
}

void GpgIdManage::populateKeysParsedInGpgIdFile()
{
    std::ifstream infile(nearestGpgIdFile);
    std::string line;
    while (std::getline(infile, line))
    {
        auto keyToSearchVector = split(line);
        if (keyToSearchVector.size() == 0)
        {
            continue;
        }

        std::string &keyToSearch = keyToSearchVector.at(0);
        std::vector<GpgKeys> keysListed = ph->listKeys(keyToSearch);
        if (keysListed.size() == 1)
        {
            keysFoundInGpgIdFile.push_back(std::move(keysListed.at(0)));
        }
        else
        {
            KeysNotFoundInGpgIdFile.push_back(line);
        }
    }
}

std::vector<std::string> GpgIdManage::split(std::string s)
{
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}
