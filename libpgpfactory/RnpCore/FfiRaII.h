#pragma once
#include "RnpCoreInterface.h"
#include "RnpCoreParams.h"
#include <iostream>
#include <rnp/rnp.h>
#include <rnp/rnp_err.h>
#include <string>

class FfiRaII
{
public:
    FfiRaII(std::string rnpHomePath, RnpCoreInterface *rnpCoreInterface);

    ~FfiRaII() { rnp_ffi_destroy(ffi); }

    void setPasswordCallback(std::function<std::string(std::string s)> func);

    RnpCoreInterface *rnpCoreInterface;

    const rnp_ffi_t getFfi() const { return ffi; }
    
    const std::string getRnpHomePath() const  {
        return cfg.CFG_HOMEDIR.u8string();
    }


    std::function<std::string(std::string s)> passwordCallback = [&](std::string keyid) {
        std::cout << "******** " << keyid << " PASSWORD **********\n";
        std::string pass;
        std::cin >> pass;
        return pass;
    };
    
private:
    rnp_ffi_t ffi = NULL;



    static bool example_pass_provider(rnp_ffi_t ffi,
                                      void *app_ctx,
                                      rnp_key_handle_t key,
                                      const char *pgp_context,
                                      char buf[],
                                      size_t buf_len);

    bool load_keyring(bool secret);

    bool load_keyrings(bool loadsecret);

    bool rnp_cfg_set_ks_info();

    void initPgpFactory();
    RnpCoreParams cfg{};
};
