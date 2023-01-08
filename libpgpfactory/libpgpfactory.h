#pragma once
#include <iostream>
#include <vector>
#include <gpgme.h>

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
    if (gpg_err_code(err))
    {
      std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
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

  std::unique_ptr<GpgMeKeys> getGpgMeKeys(std::vector<std::string> patterns)
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

private:
  gpgme_ctx_t ctx = nullptr;
  bool ctxInitialized = false;
  bool isArmor = false; // Produce output in text mode
  void checkCtxInitialized()
  {
    if (!ctxInitialized)
    {
      std::throw_with_nested(std::runtime_error("checkCtxInitialized"));
    }
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