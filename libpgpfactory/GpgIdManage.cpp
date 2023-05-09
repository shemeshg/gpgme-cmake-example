#include "GpgIdManage.h"
#include <iterator>

void GpgIdManage::init(std::string _currentPath, std::string _stopPath, PassHelper *_ph)
{
    currentPath = _currentPath;
    stopPath = _stopPath;
    ph = _ph;

    keysFoundInGpgIdFile.clear();
    KeysNotFoundInGpgIdFile.clear();
    allKeys.clear();
    allPrivateKeys.clear();
    encryptTo.clear();

    allKeys = ph->listKeys("");
    allPrivateKeys = ph->listKeys("", true);
    nearestGpgIdFolder = ph->getNearestGpgId(currentPath, stopPath);
    gpgPubKeysFolder = nearestGpgIdFolder + "/.public-keys";
    nearestGpgIdFile = nearestGpgIdFolder + "/.gpg-id";
    gpgPubKeysFolderExists = std::filesystem::exists(gpgPubKeysFolder);
    populateKeysParsedInGpgIdFile();

    encryptTo = {};
    for (const auto &r : keysFoundInGpgIdFile)
    {
        encryptTo.push_back(r.keyid);
    }

    classInitialized = true;
}

void GpgIdManage::importPublicKeyAndTrust(const std::string &filePath)
{
    if (!classInitialized)
    {
        std::throw_with_nested(std::runtime_error("Class not initialized"));
    }

    ph->importPublicKey(filePath, true);

    init(currentPath, stopPath, ph);
}

void GpgIdManage::ensureValidGpgIdFile()
{
    if (!classInitialized)
    {
        std::throw_with_nested(std::runtime_error("Class not initialized"));
    }

    if (KeysNotFoundInGpgIdFile.size() > 0)
    {
        std::throw_with_nested(std::runtime_error("Can not save back GpgIdFile with bad records in"));
    }
}

void GpgIdManage::saveBackGpgIdFile()
{

    ensureValidGpgIdFile();
    std::ofstream MyFile(nearestGpgIdFile);
    for (auto r : keysFoundInGpgIdFile)
    {
        MyFile << r.getKeyStr() << "\n";
    }
    MyFile.close();
}

void GpgIdManage::exportGpgIdToGpgPubKeysFolder()
{
    ensureValidGpgIdFile();
    std::filesystem::remove_all(gpgPubKeysFolder);
    std::filesystem::create_directories(gpgPubKeysFolder);
    for (const auto &r : keysFoundInGpgIdFile)
    {
        ph->exportPublicKey(r.keyid,
                            gpgPubKeysFolder + "/" + r.keyid + "_" + r.email + ".pub");
    }
}

void GpgIdManage::importAllGpgPubKeysFolder()
{
    if (!classInitialized)
    {
        std::throw_with_nested(std::runtime_error("Class not initialized"));
    }
    if (!gpgPubKeysFolderExists)
    {
        std::throw_with_nested(std::runtime_error(".public-keys not found"));
    }
    for (const auto &entry : std::filesystem::directory_iterator(gpgPubKeysFolder))
    {
        ph->importPublicKey(entry.path(), true);
    }

    init(currentPath, stopPath, ph);
}

void GpgIdManage::reEncryptFile(std::string pathFileToReEncrypt, bool doSign)
{
    ensureValidGpgIdFile();
    ph->reEncryptFile(pathFileToReEncrypt, encryptTo,doSign);
}

void GpgIdManage::reEncryptStoreFolder(std::function<void (std::string)> func, bool doSign)
{

    for (const std::filesystem::directory_entry &dir_entry :
         std::filesystem::recursive_directory_iterator(nearestGpgIdFolder))
    {
        if (!dir_entry.is_directory() && std::filesystem::path(dir_entry.path()).extension() == ".gpg")
        {
            func(dir_entry.path());
            // std::cout << "Re-encrypt " << dir_entry.path() << "\n";
            reEncryptFile(dir_entry.path(), doSign);
            // std::cout << " Finished\n";
        }
    }
}

void GpgIdManage::populateKeyFromString(const std::string &line)
{
    std::string keyToSearch = split(line);
    if (keyToSearch.size() == 0)
    {
        return;
    }


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

void GpgIdManage::populateKeysParsedInGpgIdFile()
{
    std::ifstream infile(nearestGpgIdFile);
    std::string line;
    while (std::getline(infile, line))
    {
        populateKeyFromString(line);
    }
}

std::string GpgIdManage::split(std::string str)
{
    // Find the first occurrence of # in the string.
    size_t pos = str.find_first_of('#');

    // Extract the substring before the #.
    std::string before = str.substr(0, pos);

    // Print the results.
    trim(before);
    return before;
}
