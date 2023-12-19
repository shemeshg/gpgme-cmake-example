#include "libpasshelper.h"
#include "InterfacePassHelper.h"

std::unique_ptr<InterfaceLibgpgfactory>  getInterfacePassHelper(){
    auto passHelper = std::unique_ptr<InterfaceLibgpgfactory>(new PassHelper);
    
    return passHelper;
}