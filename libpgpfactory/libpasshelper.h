#pragma once
#include <ostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include "libpgpfactory.h"
#include "FileSearch.h"

class PassFile
{
public:
  PassFile(std::string fullPath, GpgFactory *g);

  bool isGpgFile();

  void decrypt();

  std::string &getDecrypted();

  std::string const &getFullPath()
  {
    return fullPath;
  }

  std::string getDecryptedSignedBy()
  {
    std::ostringstream imploded;

    return decryptedSignedBy.empty() ? "" :                                          /* leave early if there are no items in the list */
               std::accumulate(                                                      /* otherwise, accumulate */
                               ++decryptedSignedBy.begin(), decryptedSignedBy.end(), /* the range 2nd to after-last */
                               *decryptedSignedBy.begin(),                           /* and start accumulating with the first item */
                               [](auto &a, auto &b)
                               { return a + "," + b; });
  }

private:
  std::string fullPath, decrypted;
  std::vector<std::string> decryptedSignedBy = {};
  GpgFactory *g;
};

class PassHelper
{
public:
  PassHelper()
  {
    g.initPgpFactory();
  };

  GpgFactory g{};

  std::unique_ptr<PassFile> getPassFile(std::string fullPath)
  {
    return std::make_unique<PassFile>(fullPath, &g);
  }

  void exportPublicKey(const std::string &keyId, const std::string &filePath)
  {
    g.exportPublicKey(keyId, filePath);
  }

  void importPublicKey(const std::string &filePath, bool doTrust)
  {
    g.importPublicKey(filePath, doTrust);
  }

  void trustPublicKey(std::string const &keyId)
  {
    g.trustPublicKey(keyId);
  }

  std::string getNearestGit(std::string currentPath, std::string stopPath)
  {
    return fileSearch.searchUp(".git", currentPath, stopPath);
  }

  std::string getNearestGpgId(std::string currentPath, std::string stopPath)
  {
    return fileSearch.searchUp(".gpg-id", currentPath, stopPath);
  }

  std::vector<GpgKeys> listKeys(std::string pattern)
  {
    return g.listKeys(pattern);
  }

  void reEncryptFile(std::string pathFileToReEncrypt, std::vector<std::string> encryptTo)
  {
    std::string backupFile = pathFileToReEncrypt + "backup";
    std::filesystem::rename(pathFileToReEncrypt,backupFile);
    PgpmeDataRII ein{backupFile, FROM_FILENAME},
        emem{},
        eout{pathFileToReEncrypt, TO_FILENAME};
    g.decryptValidate(ein, emem, false);
    emem.getString(); //get fseek to end of buffer
    g.encryptSign(emem, eout, encryptTo, true);
    std::filesystem::remove(backupFile);     
  }

private:
  FileSearch fileSearch{};
};
