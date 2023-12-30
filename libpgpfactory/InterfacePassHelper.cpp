#include "libpasshelper.h"
#include "InterfacePassHelper.h"

std::unique_ptr<InterfaceLibgpgfactory>  getInterfacePassHelper(){
    auto passHelper = std::make_unique<PassHelper>();
    
    return passHelper;
}