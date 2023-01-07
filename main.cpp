#include <iostream>
#include <gpgme.h>

#include "libpgpfactory.h"





int main(int, char **)
{
    GpgFactory g{};
    std::cout << "Hello, world!\n";
    g.listKeys();
}
