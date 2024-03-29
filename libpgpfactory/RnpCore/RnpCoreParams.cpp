#include "RnpCoreParams.h"
#include <rnp/rnp.h>
#include <iostream>

RnpCoreParams::RnpCoreParams()
{
    CFG_KR_PUB_FORMAT = RNP_KEYSTORE_GPG;
    CFG_KR_SEC_FORMAT = RNP_KEYSTORE_GPG;
    CFG_HOMEDIR = getHomeFolder();
}

std::string RnpCoreParams::getHomeFolder()
{
#ifdef WIN32
    std::string path = std::string(std::getenv("APPDATA")) + "\\gnupg";
    // use the path string as needed
    return path;
#else
    std::string path = 
    std::string(getenv("HOME")) + "/.gnupg";
    return path;
#endif
}
