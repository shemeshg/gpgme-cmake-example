#pragma once
#include <iostream>
#include <gpgme.h>

class GpgFactory
{
public:
  GpgFactory()
  { 

  }

  void initPgpFactory(){
    if (ctxInitialized) {return;}
    init_gpgme(GPGME_PROTOCOL_OpenPGP);
    gpgme_error_t err = gpgme_new(&ctx);
    gpgme_check_version(NULL);
    if (gpg_err_code(err))
    {
      std::throw_with_nested( std::runtime_error(gpgme_strerror(err)) );
    }
    ctxInitialized = true;
  }

  ~GpgFactory()
  {
    if (!ctxInitialized) {return;}
    gpgme_release(ctx);
  }

  int listKeys()
  {
    checkCtxInitialized();
    gpgme_key_t key;
    gpgme_error_t err = gpgme_op_keylist_start(ctx, "", 0);
    while (!err)
    {
      err = gpgme_op_keylist_next(ctx, &key);
      if (err)
        break;
      printf("%s:", key->subkeys->keyid);
      if (key->uids && key->uids->name)
        printf(" %s", key->uids->name);
      if (key->uids && key->uids->email)
        printf(" <%s>", key->uids->email);
      putchar('\n');
      gpgme_key_release(key);
    }

    if (gpg_err_code(err) != GPG_ERR_EOF)
    {
      std::throw_with_nested( std::runtime_error(gpgme_strerror(err)) );
    }
    return 0;
  }

private:
  gpgme_ctx_t ctx;
  bool ctxInitialized = false;
  void checkCtxInitialized(){
    if (!ctxInitialized) {
      std::throw_with_nested( std::runtime_error("checkCtxInitialized") );
    }
  }
  
  void init_gpgme(gpgme_protocol_t proto)
  {
    gpgme_error_t err;

    gpgme_check_version(NULL);
    setlocale(LC_ALL, "");
    gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
#ifndef HAVE_W32_SYSTEM
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
#endif

    err = gpgme_engine_check_version(proto);
    if (err)
    {
      fprintf(stderr, "can not init_gpgme: %s\n", gpgme_strerror(err));
      exit(1);
    }
  }
};