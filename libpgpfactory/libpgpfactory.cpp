#include "libpgpfactory.h"

void failIfErr(gpgme_error_t &err)
{
    if (gpg_err_code(err))
    {
        std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
}


PgpmeDataRII::PgpmeDataRII()
{
    gpgme_error_t err = gpgme_data_new(&d);
    if (gpg_err_code(err))
    {
        std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
}

std::string PgpmeDataRII::getString()
{
    std::stringstream stream;

    std::function<void(int, char *)> func = [&](int ret, char *buf)
    {
        stream << std::string(buf).substr(0, ret);
    };

    getData(func);
    return stream.str();
}

void PgpmeDataRII::PrintStdout()
{
    std::function<void(int, char *)> func = [&](int ret, char *buf)
    {
        fwrite(buf, ret, 1, stdout);
    };

    getData(func);
}

void PgpmeDataRII::writeToFile(std::string fileName)
{
    std::ofstream myfile(fileName);
    if (myfile.is_open())
    {
        std::function<void(int, char *)> func = [&](int ret, char *buf)
        {
            myfile << std::string(buf).substr(0, ret);
        };
        getData(func);
        myfile.close();
    }
    else
    {
        std::throw_with_nested(std::runtime_error("Unable to open file"));
    }
}

PgpmeDataRII::~PgpmeDataRII()
{
    gpgme_data_release(d);
    closeFiles();
}

void PgpmeDataRII::closeFiles()
{
    if (oBuffFileIsOpened)
    {
        fclose(oBuffFile);
    }
    if (iBuffFileIsOpened)
    {
        fclose(iBuffFile);
    }
}

void PgpmeDataRII::getData(std::function<void (int, char *)> func)
{

#define BUF_SIZE 512
    char buf[BUF_SIZE + 1];
    int ret;

    ret = gpgme_data_seek(d, 0, SEEK_SET);
    if (ret)
        std::throw_with_nested(std::runtime_error(std::to_string(errno)));
    while ((ret = gpgme_data_read(d, buf, BUF_SIZE)) > 0)
        func(ret, buf);
    if (ret < 0)
        std::throw_with_nested(std::runtime_error(std::to_string(errno)));

    /* Reset read position to the beginning so that dh can be used as input
       for another operation after this method call. For example, dh is an
       output from encryption and also is used as an input for decryption.
       Otherwise GPG_ERR_NO_DATA is returned since this method moves the
       read position. */
    ret = gpgme_data_seek(d, 0, SEEK_SET);
}

PgpmeDataRII::PgpmeDataRII(std::string s, PgmeDataInitType typ)
{
    if (typ == FROM_STRING)
    {
        gpgme_error_t err = gpgme_data_new_from_mem(&d, s.c_str(), s.size(), 0);
        if (gpg_err_code(err))
        {
            std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
        }
    }
    else if (typ == FROM_FILENAME)
    {
        iBuffFile = fopen(s.c_str(), "r");

        if (iBuffFile)
        {
            iBuffFileIsOpened = true;
            gpgme_error_t err = gpgme_data_new_from_stream(&d, iBuffFile);
            failIfErr(err);
        }
        else
        {
            std::throw_with_nested(std::runtime_error("Unable to open file"));
        }
    }
    else if (typ == TO_FILENAME)
    {
        oBuffFile = fopen(s.c_str(), "w+");

        if (oBuffFile)
        {
            oBuffFileIsOpened = true;
            gpgme_error_t err = gpgme_data_new_from_stream(&d, oBuffFile);
            failIfErr(err);
        }
        else
        {
            std::throw_with_nested(std::runtime_error("Unable to open file"));
        }
    }
}

GpgMeKeys::~GpgMeKeys()
{
    for (auto r : gpgmeKeys)
    {
        gpgme_key_unref(r);
    }
}

std::string GpgKeys::getKeyStr()
{
    return keyid + " # " + name + " <" + email + ">";
}

void GpgFactory::initPgpFactory()
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

GpgFactory::~GpgFactory()
{
    if (!ctxInitialized)
    {
        return;
    }
    gpgme_release(ctx);
}

std::unique_ptr<GpgMeKeys> GpgFactory::getGpgMeKeys(std::vector<std::string> &patterns)
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

void GpgFactory::exportPublicKey(std::string const &keyId, std::string const &filePath){
        PgpmeDataRII dout{filePath,TO_FILENAME};
        gpgme_error_t err = gpgme_op_export ( ctx, keyId.c_str(),0, dout.d);
        failIfErr(err);
}

void GpgFactory::importPublicKey(std::string const &filePath){
        PgpmeDataRII din{filePath,FROM_FILENAME};
        gpgme_error_t err = gpgme_op_import ( ctx, din.d);
        failIfErr(err);
}

void GpgFactory::decryptValidate(PgpmeDataRII &in, PgpmeDataRII &out, bool doValidate)
{

    if (doValidate)
    {
        gpgme_error_t err = gpgme_op_decrypt_verify(ctx, in.d, out.d);
        failIfErr(err);
    }
    else
    {
        gpgme_error_t err = gpgme_op_decrypt(ctx, in.d, out.d);
        failIfErr(err);
    }
    gpgme_decrypt_result_t decrypt_result = gpgme_op_decrypt_result(ctx);
    if (decrypt_result->unsupported_algorithm)
    {
        std::throw_with_nested(std::runtime_error("unsupported algorithm: " +
                                                  std::string(decrypt_result->unsupported_algorithm) + "\n"));
    }
    
    gpgme_recipient_t recipient = decrypt_result->recipients;
    while (recipient) {
        out.decryptedSignedBy.push_back(recipient->keyid);
        recipient = recipient->next;
    }

}

void GpgFactory::encryptSign(PgpmeDataRII &in, PgpmeDataRII &out, std::vector<std::string> encryptTo, bool doSign)
{
    auto gpgmeKeysTo = getGpgMeKeys(encryptTo);

    gpgme_key_t *key = &gpgmeKeysTo->gpgmeKeys[0];
    if (doSign)
    {
        gpgme_error_t err = gpgme_op_encrypt_sign(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, in.d, out.d);
        failIfErr(err);
    }
    else
    {
        gpgme_error_t err = gpgme_op_encrypt(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, in.d, out.d);
        failIfErr(err);
    }

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
    if (doSign)
    {
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
}

void GpgFactory::setCtxSigners(std::vector<std::string> signedBy)
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

std::vector<GpgKeys> GpgFactory::listKeys(const std::string pattern)
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
        setGpgKeysFromGpgme_key_t(k, key,retKeys);
        gpgme_key_unref(key);
    }

    if (gpg_err_code(err) != GPG_ERR_EOF)
    {
        std::throw_with_nested(std::runtime_error(gpgme_strerror(err)));
    }
    return retKeys;
}

void GpgFactory::setArmor(bool t)
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

void GpgFactory::setTextmode(bool t)
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

void GpgFactory::checkCtxInitialized()
{
    if (!ctxInitialized)
    {
        std::throw_with_nested(std::runtime_error("checkCtxInitialized"));
    }
}

void GpgFactory::check_sign_result(gpgme_key_t key, gpgme_sign_result_t result, gpgme_sig_mode_t type, char *fpr)
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

void GpgFactory::init_gpgme(gpgme_protocol_t proto)
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
