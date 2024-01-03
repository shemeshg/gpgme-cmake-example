#include "RnpCoreInterface.h"
#include "RnpCoreBal.h"
std::unique_ptr<RnpCoreInterface> getRnpCoreInterface(std::string rnpHomePath){
    auto rc= std::make_unique<RnpCoreBal>(rnpHomePath);
    return rc;
}