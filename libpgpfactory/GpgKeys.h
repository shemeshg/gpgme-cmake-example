#pragma once
#include <string>

class GpgKeys
{
public:
  bool can_encrypt = false, invalid = false;
  std::string keyid, name, email, foundUsingPattern;
  int validity;
  std::string getKeyStr();
};
