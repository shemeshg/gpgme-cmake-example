#pragma once

#include <vector>
#include <gpgme.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <functional>

void failIfErr(gpgme_error_t &err);

enum PgmeDataInitType
{
  FROM_STRING,
  FROM_FILENAME,
  TO_FILENAME
};

class PgpmeDataRII
{
public:
  gpgme_data_t d = NULL;

  PgpmeDataRII();

  PgpmeDataRII(std::string s, PgmeDataInitType typ);

  PgpmeDataRII(PgpmeDataRII const &) = delete;
  PgpmeDataRII &operator=(PgpmeDataRII const &) = delete;
  PgpmeDataRII(PgpmeDataRII &&) = delete;
  PgpmeDataRII &operator=(PgpmeDataRII &&) = delete;

  std::string getString();

  void PrintStdout();

  void writeToFile(std::string fileName);

  ~PgpmeDataRII();

  void closeFiles();

  std::vector<std::string> decryptedSignedBy = {};
  
protected:
  void getData(std::function<void(int, char *)> func);

private:
  FILE *oBuffFile;
  bool oBuffFileIsOpened = false;
  FILE *iBuffFile;
  bool iBuffFileIsOpened = false;
};

class GpgMeKeys
{
public:
  GpgMeKeys() = default;

  GpgMeKeys(GpgMeKeys const &) = delete;
  GpgMeKeys &operator=(GpgMeKeys const &) = delete;
  GpgMeKeys(GpgMeKeys &&) = delete;
  GpgMeKeys &operator=(GpgMeKeys &&) = delete;

  std::vector<gpgme_key_t> gpgmeKeys = {};
  ~GpgMeKeys();
};

class GpgKeys
{
public:
  bool can_encrypt = false, invalid = false;
  std::string keyid, name, email, foundUsingPattern;
  int validity;
  std::string getKeyStr();
};

class GpgFactory
{
public:
  GpgFactory() = default;

  GpgFactory(GpgFactory const &) = delete;
  GpgFactory &operator=(GpgFactory const &) = delete;
  GpgFactory(GpgFactory &&) = delete;
  GpgFactory &operator=(GpgFactory &&) = delete;

  void initPgpFactory();

  ~GpgFactory();

  std::unique_ptr<GpgMeKeys> getGpgMeKeys(std::vector<std::string> &patterns, bool onlyPrivateKey);

  void decryptValidate(PgpmeDataRII &in, PgpmeDataRII &out, bool doValidate);

  void encryptSign(PgpmeDataRII &in, PgpmeDataRII &out, std::vector<std::string> encryptTo, bool doSign);

  void exportPublicKey(std::string const &keyId, std::string const &filePath);
  void importPublicKey(std::string const &filePath, bool doTrust);
  void trustPublicKey(std::string const &keyId);  

  void setCtxSigners(std::vector<std::string> signedBy);

  std::vector<GpgKeys> listKeys(const std::string pattern = "");

  void setArmor(bool t);

  void setTextmode(bool t);

private:
  gpgme_ctx_t ctx = nullptr;
  bool ctxInitialized = false;
  bool isArmor = false;
  bool isTextmode = false;
  std::unique_ptr<GpgMeKeys> pgpMeKeysSigners = std::make_unique<GpgMeKeys>();
  void checkCtxInitialized();

  void
  check_sign_result(gpgme_key_t key, gpgme_sign_result_t result,
                    gpgme_sig_mode_t type, char *fpr);

  void init_gpgme(gpgme_protocol_t proto);
  void setGpgKeysFromGpgme_key_t(GpgKeys &k, gpgme_key_t key,std::vector<GpgKeys> &retKeys){
        k.can_encrypt = key->subkeys->can_encrypt;
        k.keyid = key->subkeys->keyid;
        k.name = key->uids->name;
        k.email = key->uids->email;
        k.invalid = key->uids->invalid;
        k.validity = key->uids->validity;
        retKeys.push_back(std::move(k));
  }
};
