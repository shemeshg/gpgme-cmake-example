#pragma once
#include <iostream>
#include <vector>
#include <gpgme.h>

class PgpmeDataRII
{
public:
  gpgme_data_t d;
  PgpmeDataRII()
  {
    gpgme_error_t err = gpgme_data_new(&d);
    if (gpg_err_code(err))
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
  }

  PgpmeDataRII(std::string s)
  {
    gpgme_error_t err = gpgme_data_new_from_mem(&d, s.c_str(), s.size(), 0);
    if (gpg_err_code(err))
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
  }


  ~PgpmeDataRII()
  {
    gpgme_data_release(d);
  }
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
  ~GpgMeKeys()
  {
    for (auto r : gpgmeKeys)
    {
      gpgme_key_unref(r);
    }
  }
};

class GpgKeys
{
public:
  bool can_encrypt = false, invalid = false;
  std::string keyid, name, email;
  std::string getKeyStr()
  {
    return keyid + " # " + name + " <" + email + ">";
  }
};

class GpgFactory
{
public:
  GpgFactory() = default;

  GpgFactory(GpgFactory const &) = delete;
  GpgFactory &operator=(GpgFactory const &) = delete;
  GpgFactory(GpgFactory &&) = delete;
  GpgFactory &operator=(GpgFactory &&) = delete;

  void initPgpFactory()
  {
    if (ctxInitialized)
    {
      return;
    }
    init_gpgme(GPGME_PROTOCOL_OpenPGP);
    gpgme_error_t err = gpgme_new(&ctx);
    gpgme_check_version(NULL);
    failIfErr(err);
    ctxInitialized = true;
  }

  ~GpgFactory()
  {
    if (!ctxInitialized)
    {
      return;
    }
    gpgme_release(ctx);
  }

  std::unique_ptr<GpgMeKeys> getGpgMeKeys(std::vector<std::string> &patterns)
  {
    checkCtxInitialized();
    std::unique_ptr<GpgMeKeys> gmk = std::make_unique<GpgMeKeys>();

    for (auto r : patterns)
    {
      gpgme_key_t key = nullptr;
      gpgme_error_t err = gpgme_get_key(ctx, r.c_str(), &key, 1);
      if (gpg_err_code(err))
      {
        std::throw_with_nested(std::runtime_error(r + ": " + gpgme_strerror(err)));
      }
      else
      {
        gmk->gpgmeKeys.push_back(key);
      }
    }
    gmk->gpgmeKeys.push_back(NULL);
    return gmk;
  }

  void
  encryptSign()
  {
    
    std::vector<std::string> encryptTo = {"shemeshg"};
    auto gpgmeKeysTo = getGpgMeKeys(encryptTo);
    PgpmeDataRII in{}, out{"shalom olam\nabc\n"};
    
    gpgme_key_t *key = &gpgmeKeysTo->gpgmeKeys[0];
    gpgme_error_t err = gpgme_op_encrypt_sign(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, in.d, out.d);
    failIfErr(err);

    /* Reset read position to 0 on in otherwise GPG_ERR_NO_DATA would occur when
        gpgme_op_sign() is called consecutively. */
    gpgme_data_seek(in.d, 0, SEEK_SET);
    /* Reset read position to 0 on out otherwise GPG_ERR_NO_DATA would occur when
       gpgme_op_verify() is called on out consecutively. */
    gpgme_data_seek(out.d, 0, SEEK_SET);
    gpgme_encrypt_result_t result = gpgme_op_encrypt_result(ctx);
    if (result->invalid_recipients)
    {
      std::throw_with_nested(std::runtime_error("Invalid recipient encountered: " + std::string(result->invalid_recipients->fpr) + "\n"));
    }
    gpgme_sign_result_t sign_result = gpgme_op_sign_result(ctx);

    for (auto r : pgpMeKeysSigners->gpgmeKeys)
    {
      if (r != NULL)
      {
        check_sign_result(r, sign_result, GPGME_SIG_MODE_NORMAL,
                          r->subkeys->next->fpr);
      }
    }
  }

  void setCtxSigners(std::vector<std::string> signedBy)
  {

    checkCtxInitialized();
    /* Include signature within key. */
    gpgme_keylist_mode_t kmode = gpgme_get_keylist_mode(ctx);
    kmode |= GPGME_KEYLIST_MODE_SIGS;
    gpgme_error_t err = gpgme_set_keylist_mode(ctx, kmode);
    failIfErr(err);
    gpgme_signers_clear(ctx);
    pgpMeKeysSigners = getGpgMeKeys(signedBy);
    for (auto r : pgpMeKeysSigners->gpgmeKeys)
    {
      if (r != NULL)
      {
        gpgme_signers_add(ctx, r);
      }
    }
  }

  std::vector<GpgKeys> listKeys(const std::string pattern = "")
  {
    std::vector<GpgKeys> retKeys = {};

    checkCtxInitialized();
    gpgme_key_t key = nullptr;
    gpgme_error_t err = gpgme_op_keylist_start(ctx, pattern.c_str(), 0);
    while (!err)
    {
      err = gpgme_op_keylist_next(ctx, &key);
      if (err)
        break;
      GpgKeys k;
      k.can_encrypt = key->subkeys->can_encrypt;
      k.keyid = key->subkeys->keyid;
      k.name = key->uids->name;
      k.email = key->uids->email;
      k.invalid = key->uids->invalid;
      retKeys.push_back(std::move(k));
      k.invalid = key->uids->invalid;
      gpgme_key_unref(key);
    }

    if (gpg_err_code(err) != GPG_ERR_EOF)
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
    return retKeys;
  }

  void setArmor(bool t)
  {
    checkCtxInitialized();
    if (t)
    {
      gpgme_set_armor(ctx, 1);
      isArmor = true;
    }
    else
    {
      gpgme_set_armor(ctx, 0);
      isArmor = false;
    }
  }

  void setTextmode(bool t)
  {
    checkCtxInitialized();
    if (t)
    {
      gpgme_set_textmode(ctx, 1);
      isTextmode = true;
    }
    else
    {
      gpgme_set_textmode(ctx, 0);
      isTextmode = false;
    }
  }

private:
  gpgme_ctx_t ctx = nullptr;
  bool ctxInitialized = false;
  bool isArmor = false;
  bool isTextmode = false;
  std::unique_ptr<GpgMeKeys> pgpMeKeysSigners = std::make_unique<GpgMeKeys>();
  void checkCtxInitialized()
  {
    if (!ctxInitialized)
    {
      std::throw_with_nested(std::runtime_error("checkCtxInitialized"));
    }
  }

  void failIfErr(gpgme_error_t &err)
  {
    if (gpg_err_code(err))
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
  }

  void
  check_sign_result(gpgme_key_t key, gpgme_sign_result_t result,
                    gpgme_sig_mode_t type, char *fpr)
  {
    gpgme_key_sig_t sig = key->uids->signatures;
    if (result->invalid_signers)
    {
      std::string str = "Invalid signer found: " + std::string(result->invalid_signers->fpr) + "\n";
      std::throw_with_nested(std::runtime_error(str));
    }
    if (!result->signatures || result->signatures->next)
    {
      std::string str = "Unexpected number of signatures created\n";
      std::throw_with_nested(std::runtime_error(str));
    }
    if (result->signatures->type != type)
    {
      std::throw_with_nested(std::runtime_error("Wrong type of signature created\n"));
    }
    if (result->signatures->pubkey_algo != sig->pubkey_algo)
    {
      std::string str = "Wrong pubkey algorithm reported: \n";
      std::throw_with_nested(std::runtime_error(str));
    }
    /*
    if (strcmp(fpr, result->signatures->fpr))
    {
      std::string str = "Wrong fingerprint reported: " + std::string(result->signatures->fpr) + "is not " + fpr + "\n";
      std::throw_with_nested(std::runtime_error(str));
    }
    */
  }

  void init_gpgme(gpgme_protocol_t proto)
  {
    gpgme_error_t err = 0;

    gpgme_check_version(NULL);
    setlocale(LC_ALL, "");
    gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
#ifndef HAVE_W32_SYSTEM
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
#endif

    err = gpgme_engine_check_version(proto);
    if (err)
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
  }
};