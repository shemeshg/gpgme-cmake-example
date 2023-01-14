#pragma once
#include <iostream>
#include <filesystem>
#include "libpgpfactory.h"

class PassFile
{
public:
  PassFile(std::string fullPath, GpgFactory *g);

  bool isGpgFile();

  void decrypt();

  std::string &getDecrypted();

private:
  std::string fullPath, decrypted;
  GpgFactory *g;
};

class PassHelper {
  public:
  PassHelper(){
    g.initPgpFactory(); 
  };

 std::unique_ptr<PassFile> getPassFile(std::string fullPath){
    return std::make_unique<PassFile>(fullPath, &g);
  }

  GpgFactory g{};

};
