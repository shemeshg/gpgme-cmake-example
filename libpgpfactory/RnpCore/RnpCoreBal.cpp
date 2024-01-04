#include "RnpCoreDefinitions.h"
#include <filesystem>
#include <map>

#include "RnpCoreBal.h"

class RnpInputFromPath
{
public:
    RnpInputFromPath(const std::string &filePath)
        : filePath{filePath}
    {
        rnp_input_from_path(&input, filePath.c_str());
    }

    const rnp_input_t *getInput() const { return &input; }

    virtual ~RnpInputFromPath() { rnp_input_destroy(input); }

private:
    std::string filePath;
    rnp_input_t input = NULL;
};

class RnpOutputToPath
{
public:
    RnpOutputToPath(const std::string &filePath)
        : filePath{filePath}
    {
        rnp_output_to_path(&output, filePath.c_str()); 
    }

    const rnp_output_t *getOutput() const { return &output; }

    virtual ~RnpOutputToPath() { rnp_output_destroy(output); }

private:
    std::string filePath;
    rnp_output_t output = NULL;
};


class RnpOutputToMemory
{
    public:
    RnpOutputToMemory() { rnp_output_to_memory(&output, 0); }

    const rnp_output_t *getOutput() const { return &output; }

    virtual ~RnpOutputToMemory() { rnp_output_destroy(output); }

private:
    rnp_output_t output = NULL;
};

class RnpInputFromMemory
{
    public:
    RnpInputFromMemory(const std::string &inStr) { 
        message = inStr.c_str();
        rnp_input_from_memory(&input, (uint8_t *) message, strlen(message), false); 
        }

    const rnp_input_t *getInput() const { return &input; }

    virtual ~RnpInputFromMemory() { rnp_input_destroy(input); }

private:
    rnp_input_t input = NULL;
    const char *message;
};


RnpCoreBal::~RnpCoreBal()
{}

RnpCoreBal::RnpCoreBal(std::string rnpHomePath)
{
    ffiRaII = std::make_unique<FfiRaII>(rnpHomePath, this);
}

void RnpCoreBal::decryptFileToString(const std::string &filePath,
                                     std::string &decrypted,
                                     std::vector<std::string> &decryptedSignedBy)
{
    RnpInputFromPath input{filePath};
    RnpOutputToMemory output{};
    uint8_t *buf = NULL;
    size_t buf_len = 0;


    rnp_op_verify_t verify = NULL;

    r([&]() { return rnp_op_verify_create(&verify, ffiRaII->getFfi(),
     *input.getInput(), *output.getOutput()); });

    lastKeyIdRequested = "klkl";
    r_pass([&]() { return rnp_op_verify_execute(verify); },
           RnpLoginRequestException(1002,
                                    "Rnp Login Request",
                                    "decryptFileToString",
                                    lastKeyIdRequested,
                                    filePath,
                                    "",
                                    {},
                                    false));

    size_t sigcount = 0;

    r([&]() { return rnp_op_verify_get_signature_count(verify, &sigcount); });

    // get the decrypted message from the output structure
    r([&]() { return rnp_output_memory_get_buf(*output.getOutput(), &buf, &buf_len, false); });

    decrypted = std::string(buf, buf + buf_len);
    for (size_t i = 0; i < sigcount; i++) {
        rnp_op_verify_signature_t sig = NULL;
        rnp_result_t sigstatus = RNP_SUCCESS;
        rnp_key_handle_t key = NULL;
        char *keyid = NULL;

        if (rnp_op_verify_get_signature_at(verify, i, &sig) != RNP_SUCCESS) {
            throw std::runtime_error("failed to get signature " + std::to_string(i) + "\n");
        }

        if (rnp_op_verify_signature_get_key(sig, &key) != RNP_SUCCESS) {
            throw std::runtime_error("failed to get signature's " + std::to_string(i) + " key\n");
        }

        if (rnp_key_get_keyid(key, &keyid) != RNP_SUCCESS) {
            rnp_key_handle_destroy(key);
            throw std::runtime_error("failed to get key id " + std::to_string(i) + "\n");
        }

        sigstatus = rnp_op_verify_signature_get_status(sig);
        decryptedSignedBy.push_back(keyid);
        rnp_buffer_destroy(keyid);
        rnp_key_handle_destroy(key);
    }

}

void RnpCoreBal::decryptFileToFile(const std::string &fromFilePath, const std::string &toFilePath)
{
    RnpInputFromPath input{fromFilePath};
    RnpOutputToPath output{toFilePath};
    uint8_t *buf = NULL;
    size_t buf_len = 0;

    r_pass([&]() { return rnp_decrypt(ffiRaII->getFfi(), *input.getInput(), 
        *output.getOutput()); },
           RnpLoginRequestException(1002,
                                    "Rnp Login Request",
                                    "decryptFileToFile",
                                    lastKeyIdRequested,
                                    fromFilePath,
                                    toFilePath,
                                    {},
                                    false));

}

void RnpCoreBal::encryptSignStringToFile(const std::string &inStr,
                                         const std::string &outFilePath,
                                         std::vector<std::string> encryptTo,
                                         bool doSign)
{
    RnpInputFromMemory input{inStr};
    RnpOutputToPath output{outFilePath};
   

    /* create encryption operation */
    rnp_op_encrypt_t encrypt = NULL;
    if (rnp_op_encrypt_create(&encrypt, ffiRaII->getFfi(), *input.getInput(), 
    *output.getOutput()) != RNP_SUCCESS) {
        throw std::runtime_error("failed to create encrypt operation\n");
    }

    /* setup encryption parameters */
    rnp_op_encrypt_set_armor(encrypt, isArmor);
    rnp_op_encrypt_set_file_name(encrypt, outFilePath.c_str());
    rnp_op_encrypt_set_file_mtime(encrypt, (uint32_t) time(NULL));
    // rnp_op_encrypt_set_compression(encrypt, "ZIP", 6);
    rnp_op_encrypt_set_cipher(encrypt, RNP_ALGNAME_AES_256);
    rnp_op_encrypt_set_aead(encrypt, "None");

    for (auto &eTo : encryptTo) {
        rnp_key_handle_t key = NULL;
        if (rnp_locate_key(ffiRaII->getFfi(), "keyid", eTo.c_str(), &key) != RNP_SUCCESS) {
            throw std::runtime_error("failed to locate recipient key rsa@key.\n");
        }

        if (rnp_op_encrypt_add_recipient(encrypt, key) != RNP_SUCCESS) {
            throw std::runtime_error("failed to add recipient\n");
        }
        rnp_key_handle_destroy(key);
    }

    std::vector<rnp_key_handle_t> signkeys;
    if (doSign && !signer_fingerprint.empty()) {
        int flags = CLI_SEARCH_SECRET | CLI_SEARCH_DEFAULT | CLI_SEARCH_SUBKEYS
                    | CLI_SEARCH_FIRST_ONLY;
        if (!keys_matching(signkeys, signer_fingerprint.c_str(), flags)) {
            throw std::runtime_error("Key(s) not found.\n");
        }
        for (rnp_key_handle_t key : signkeys) {
            if (rnp_op_encrypt_add_signature(encrypt, key, NULL)) {
                throw std::runtime_error("Failed to add signature");
            }
        }
    }

    r_pass([&]() { return rnp_op_encrypt_execute(encrypt); },
           RnpLoginRequestException(1003,
                                    "Rnp Login Request",
                                    "encryptSignStringToFile",
                                    lastKeyIdRequested,
                                    inStr,
                                    outFilePath,
                                    encryptTo,
                                    doSign));

    clear_key_handles(signkeys);
    rnp_op_encrypt_destroy(encrypt);
}

void RnpCoreBal::encryptSignFileToFile(const std::string &inFilePath,
                                       const std::string &outFilePath,
                                       std::vector<std::string> encryptTo,
                                       bool doSign)
{
    RnpInputFromPath input{inFilePath};
    RnpOutputToPath output{outFilePath};

    /* create encryption operation */
    rnp_op_encrypt_t encrypt = NULL;
    if (rnp_op_encrypt_create(&encrypt, ffiRaII->getFfi(), *input.getInput(), 
    *output.getOutput()) != RNP_SUCCESS) {
        throw std::runtime_error("failed to create encrypt operation\n");
    }

    /* setup encryption parameters */
    rnp_op_encrypt_set_armor(encrypt, isArmor);
    rnp_op_encrypt_set_file_name(encrypt, outFilePath.c_str());
    rnp_op_encrypt_set_file_mtime(encrypt, (uint32_t) time(NULL));
    // rnp_op_encrypt_set_compression(encrypt, "ZIP", 6);
    rnp_op_encrypt_set_cipher(encrypt, RNP_ALGNAME_AES_256);
    rnp_op_encrypt_set_aead(encrypt, "None");

    for (auto &eTo : encryptTo) {
        rnp_key_handle_t key = NULL;
        if (rnp_locate_key(ffiRaII->getFfi(), "keyid", eTo.c_str(), &key) != RNP_SUCCESS) {
            throw std::runtime_error("failed to locate recipient key rsa@key.\n");
        }

        if (rnp_op_encrypt_add_recipient(encrypt, key) != RNP_SUCCESS) {
            throw std::runtime_error("failed to add recipient\n");
        }
        rnp_key_handle_destroy(key);
    }

    if (doSign) {
        rnp_key_handle_t key = NULL;
        if (!signer_fingerprint.empty()) {
            if (rnp_locate_key(ffiRaII->getFfi(), "keyid", signer_fingerprint.c_str(), &key)
                != RNP_SUCCESS) {
                throw std::runtime_error("failed to locate recipient key rsa@key.\n");
            }
        }
        rnp_op_encrypt_add_signature(encrypt, key, NULL);
        rnp_key_handle_destroy(key);
    }

    r_pass([&]() { return rnp_op_encrypt_execute(encrypt); },
           RnpLoginRequestException(1004,
                                    "Rnp Login Request",
                                    "encryptSignFileToFile",
                                    lastKeyIdRequested,
                                    inFilePath,
                                    outFilePath,
                                    encryptTo,
                                    doSign));

    rnp_op_encrypt_destroy(encrypt);
}

void RnpCoreBal::reEncryptFile(std::string pathFileToReEncrypt,
                               std::vector<std::string> encryptTo,
                               bool doSign)
{
    std::string backupFile = pathFileToReEncrypt + "backup";
    std::string tempDecrypted = pathFileToReEncrypt + "decrypted";
    try {
        std::filesystem::rename(pathFileToReEncrypt, backupFile);

        std::vector<std::string> decryptedSignedBy{};
        decryptFileToFile(backupFile, tempDecrypted);

        encryptSignFileToFile(tempDecrypted, pathFileToReEncrypt, encryptTo, doSign);

        std::filesystem::remove(backupFile);
        std::filesystem::remove(tempDecrypted);
    } catch (...) {
        std::filesystem::rename(backupFile, pathFileToReEncrypt);
        std::filesystem::remove(tempDecrypted);
        throw;
    }
}

void RnpCoreBal::setCtxSigners(std::vector<std::string> signedBy)
{
    if (signedBy.size() > 0) {
        signer_fingerprint = signedBy[0];
    }
}

bool RnpCoreBal::ffi_export_key(rnp_ffi_t ffi,
                                const char *uid,
                                bool secret,
                                const std::string &filePath)
{
    RnpOutputToPath keyfile{filePath};
    rnp_key_handle_t key = NULL;
    uint32_t flags = RNP_KEY_EXPORT_ARMORED | RNP_KEY_EXPORT_SUBKEYS;
    char *keyid = NULL;
    bool result = false;

    /* you may search for the key via userid, keyid, fingerprint, grip */
    if (rnp_locate_key(ffi, "keyid", uid, &key) != RNP_SUCCESS) {
        return false;
    }

    if (!key) {
        return false;
    }

    /* get key's id and build filename */
    if (rnp_key_get_keyid(key, &keyid) != RNP_SUCCESS) {
        goto finish;
    }
    rnp_buffer_destroy(keyid);



    flags = flags | (secret ? RNP_KEY_EXPORT_SECRET : RNP_KEY_EXPORT_PUBLIC);
    if (rnp_key_export(key, *keyfile.getOutput(), flags) != RNP_SUCCESS) {
        goto finish;
    }

    result = true;
finish:
    rnp_key_handle_destroy(key);
    return result;
}

void RnpCoreBal::exportPublicKey(const std::string &keyId, const std::string &filePath)
{
    ffi_export_key(ffiRaII->getFfi(), keyId.c_str(), false, filePath);
}

void RnpCoreBal::importPublicKey(const std::string &filePath, bool doTrust)
{
    bool isSccussfull = import_keys(ffiRaII->getFfi(), filePath, RNP_LOAD_SAVE_PUBLIC_KEYS);
    if (!isSccussfull) {
        throw std::runtime_error("Could not import key");
    }
}

void RnpCoreBal::trustPublicKey(const std::string &keyId) {}

std::string RnpCoreBal::getPrimaryKey(std::string searchKey)
{
    static std::map<std::string, std::string> cashPrimaryKey;

    std::map<std::string, std::string>::iterator it;

    if (cashPrimaryKey.count(searchKey)) {
        return cashPrimaryKey[searchKey];
    }

    std::vector<std::string> retKeys = {};

    std::vector<rnp_key_handle_t> keys;

    int flags = true ? CLI_SEARCH_SECRET : 0;
    if (!keys_matching(keys, "", CLI_SEARCH_SUBKEYS_AFTER)) {
        throw std::runtime_error("Key(s) not found.\n");
    }

    std::vector<std::string> ret{};
    bool found = false;
    for (auto key : keys) {
        char *keyid = NULL;
        (void) rnp_key_get_keyid(key, &keyid);

        bool isPrimary = false;
        rnp_key_is_primary(key, &isPrimary);
        if (keyid == searchKey) {
            found = true;
        }
        if (isPrimary) {
            ret = {keyid};
        } else {
            ret.push_back(keyid);
        }
        if (found) {
            cashPrimaryKey[searchKey] = ret.at(0);
            return ret.at(0);
        }

        rnp_buffer_destroy(keyid);
    }
    return searchKey;
}

std::vector<RnpKeys> RnpCoreBal::listKeys(const std::string pattern, bool secret_only)
{
    std::vector<RnpKeys> retKeys = {};

    std::vector<rnp_key_handle_t> keys;

    int flags = secret_only ? CLI_SEARCH_SECRET : 0;
    if (!keys_matching(keys, pattern, flags)) {
        return retKeys;
    }

    for (auto key : keys) {
        char *keyfp = NULL;
        char *uid_str = NULL;
        char *keyid = NULL;
        (void) rnp_key_get_uid_at(key, 0, &uid_str);
        (void) rnp_key_get_fprint(key, &keyfp);
        (void) rnp_key_get_keyid(key, &keyid);

        RnpKeys gk;
        std::string nameAndEmail = uid_str;
        size_t pos_email_start = nameAndEmail.find_first_of('<');
        size_t pos_email_end = nameAndEmail.find_first_of('>');
        gk.name = nameAndEmail.substr(0, pos_email_start - 1);
        gk.email = nameAndEmail.substr(pos_email_start + 1, pos_email_end - pos_email_start - 1);
        gk.keyid = keyid;
        gk.foundUsingPattern = pattern;
        gk.can_encrypt = secret_only;
        gk.validity = 0;
        if (gk.getKeyStr().find(pattern) != std::string::npos) {
            retKeys.push_back(gk);
        }

        rnp_buffer_destroy(uid_str);
        rnp_buffer_destroy(keyfp);
        rnp_buffer_destroy(keyid);
    }

    return retKeys;
}

bool RnpCoreBal::import_keys(rnp_ffi_t ffi, const std::string &path, uint32_t flags)
{ 
    RnpInputFromPath input{path};


    bool res = !rnp_import_keys(ffi, *input.getInput(), flags, NULL);

    return res;
}

bool RnpCoreBal::import_keys(rnp_ffi_t ffi, const uint8_t *data, size_t len, uint32_t flags)
{
    rnp_input_t input = NULL;
    if (rnp_input_from_memory(&input, data, len, false)) {
        return false;
    }
    bool res = !rnp_import_keys(ffi, input, flags, NULL);
    rnp_input_destroy(input);
    return res;
}

bool RnpCoreBal::add_key_to_array(rnp_ffi_t ffi,
                                  std::vector<rnp_key_handle_t> &keys,
                                  rnp_key_handle_t key,
                                  int flags)
{
    bool subkey = false;
    bool subkeys = (flags & CLI_SEARCH_SUBKEYS_AFTER) == CLI_SEARCH_SUBKEYS_AFTER;
    if (rnp_key_is_sub(key, &subkey)) {
        return false;
    }

    try {
        keys.push_back(key);
    } catch (const std::exception &e) {
        ERR_MSG("%s", e.what());
        return false;
    }
    if (!subkeys || subkey) {
        return true;
    }

    std::vector<rnp_key_handle_t> subs;
    size_t sub_count = 0;
    if (rnp_key_get_subkey_count(key, &sub_count)) {
        goto error;
    }

    try {
        for (size_t i = 0; i < sub_count; i++) {
            rnp_key_handle_t sub_handle = NULL;
            if (rnp_key_get_subkey_at(key, i, &sub_handle)) {
                goto error;
            }
            subs.push_back(sub_handle);
        }
        std::move(subs.begin(), subs.end(), std::back_inserter(keys));
    } catch (const std::exception &e) {
        ERR_MSG("%s", e.what());
        goto error;
    }
    return true;
error:
    keys.pop_back();
    clear_key_handles(subs);
    return false;
}

void RnpCoreBal::clear_key_handles(std::vector<rnp_key_handle_t> &keys)
{
    for (auto handle : keys) {
        rnp_key_handle_destroy(handle);
    }
    keys.clear();
}

bool RnpCoreBal::key_matches_flags(rnpffi::Key &key, int flags)
{
    /* check whether secret key search is requested */
    if ((flags & CLI_SEARCH_SECRET) && !key.secret()) {
        return false;
    }
    /* check whether no subkeys allowed */
    if (!key.is_sub()) {
        return true;
    }
    if (!(flags & CLI_SEARCH_SUBKEYS)) {
        return false;
    }
    /* check whether subkeys should be put after primary (if it is available) */
    if ((flags & CLI_SEARCH_SUBKEYS_AFTER) != CLI_SEARCH_SUBKEYS_AFTER) {
        return true;
    }

    return key.primary_grip().empty();
}

bool RnpCoreBal::keys_matching(std::vector<rnp_key_handle_t> &keys,
                               const std::string &str,
                               int flags)
{
    rnpffi::FFI ffiobj(ffiRaII->getFfi(), false);

    /* iterate through the keys */
    auto it = ffiobj.iterator_create("fingerprint");
    if (!it) {
        return false;
    }

    std::string fp;
    while (it->next(fp)) {
        auto key = ffiobj.locate_key("fingerprint", fp);
        if (!key) {
            continue;
        }
        if (!key_matches_flags(*key, flags) || !key_matches_string(*key, str)) {
            continue;
        }
        if (!add_key_to_array(ffiRaII->getFfi(), keys, key->handle(), flags)) {
            return false;
        }
        key->release();
        if (flags & CLI_SEARCH_FIRST_ONLY) {
            return true;
        }
    }
    return !keys.empty();
}

bool RnpCoreBal::keys_matching(std::vector<rnp_key_handle_t> &keys, int flags)
{
    rnpffi::FFI ffiobj(ffiRaII->getFfi(), false);

    /* iterate through the keys */
    auto it = ffiobj.iterator_create("fingerprint");
    if (!it) {
        return false;
    }

    std::string fp;
    while (it->next(fp)) {
        auto key = ffiobj.locate_key("fingerprint", fp);
        if (!key) {
            continue;
        }
        if (!key_matches_flags(*key, flags)) {
            continue;
        }
        if (!add_key_to_array(ffiRaII->getFfi(), keys, key->handle(), flags)) {
            return false;
        }
        key->release();
        if (flags & CLI_SEARCH_FIRST_ONLY) {
            return true;
        }
    }
    return !keys.empty();
}

std::string RnpCoreBal::getRnpVersionString()
{
    return rnp_version_string();
}
