#include <iostream>
#include <gpgme.h>

#include "libpgpfactory.h"





int main(int, char **)
{
    GpgFactory g{};
    g.initPgpFactory();
    std::cout << "Hello, world!\n";
    g.listKeys();
}
