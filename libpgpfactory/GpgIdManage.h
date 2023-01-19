#pragma once
#include "libpasshelper.h"

class GpgIdManage
{
public:
  GpgIdManage(){}

  GpgIdManage(GpgIdManage const &) = delete;
  GpgIdManage &operator=(GpgIdManage const &) = delete;
  GpgIdManage(GpgIdManage &&) = delete;
  GpgIdManage &operator=(GpgIdManage &&) = delete;

  ~GpgIdManage(){};

  std::string currentPath, stopPath, nearestGpgIdFolder,
      gpgPubKeysFolder, nearestGpgIdFile;
  bool gpgPubKeysFolderExists = false;
  bool classInitialized = false;

  std::vector<std::string> KeysNotFoundInGpgIdFile{};
  std::vector<GpgKeys> keysFoundInGpgIdFile{};
  std::vector<GpgKeys> allKeys;
  std::vector<std::string> encryptTo{};

  void init(std::string _currentPath, std::string _stopPath, PassHelper *_ph);

  void importPublicKeyAndTrust(const std::string &filePath)
  {
    ph->importPublicKey(filePath, true);
    init( currentPath,  stopPath, ph);
  }

  void ensureValidGpgIdFile()
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

  void saveBackGpgIdFile()
  {

    ensureValidGpgIdFile();
    std::ofstream MyFile(nearestGpgIdFile);
    for (auto r : keysFoundInGpgIdFile)
    {
      MyFile << r.getKeyStr() << "\n";
    }
    MyFile.close();
  }

  void exportGpgIdToGpgPubKeysFolder()
  {
    ensureValidGpgIdFile();
    std::filesystem::remove_all(gpgPubKeysFolder);
    std::filesystem::create_directories(gpgPubKeysFolder);
    for (auto r : keysFoundInGpgIdFile)
    {
      ph->exportPublicKey(r.keyid,
                          gpgPubKeysFolder + "/" + r.keyid + "_" + r.email + ".pub");
    }
  }

  void importAllGpgPubKeysFolder()
  {
    if (!classInitialized)
    {
      std::throw_with_nested(std::runtime_error("Class not initialized"));
    }
    if (!gpgPubKeysFolderExists)
    {
      std::throw_with_nested(std::runtime_error(".gpg-pub-keys not found"));
    }
    for (const auto &entry : std::filesystem::directory_iterator(gpgPubKeysFolder))
    {
      ph->importPublicKey(entry.path(), true);
    }

    init( currentPath,  stopPath, ph);
  }

  void reEncryptFile(std::string pathFileToReEncrypt)
  {
    ensureValidGpgIdFile();
    ph->reEncryptFile(pathFileToReEncrypt, encryptTo);
  }

  void reEncryptStoreFolder()
  {
    

    for (const std::filesystem::directory_entry &dir_entry :
         std::filesystem::recursive_directory_iterator(nearestGpgIdFolder))
    {
      if (!dir_entry.is_directory() && std::filesystem::path(dir_entry.path()).extension() == ".gpg")
      {
        std::cout << "Re-encrypt " << dir_entry.path() << "\n";
        reEncryptFile(dir_entry.path());
        // std::cout << " Finished\n";
      }
    }
  }

  // hygen public
private:
  void populateKeysParsedInGpgIdFile();

  std::vector<std::string> split(std::string s);

  PassHelper *ph;

  // hygen private
};
