#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <future>

class RunShellCmd
{
public:
  RunShellCmd() {}
  std::future<std::string> runCmdAsync(std::vector<std::string> cmd)
  {
    std::future<std::string> result = std::async([=]()
                                                 { return runCmd(cmd); });
    return result;
  }

  std::string runCmd(std::vector<std::string> cmd)
  {
    std::string escapedString{};
    for (std::string s : cmd)
    {
      escapedString = escapedString + s + " ";
    }

    return exec(escapedString.c_str());
  }

private:
  char *escapeshellarg(char *str)
  {
    char *escStr;
    int i,
        count = strlen(str),
        ptr_size = count + 3;

    escStr = (char *)calloc(ptr_size, sizeof(char));
    if (escStr == NULL)
    {
      return NULL;
    }
    sprintf(escStr, "'");

    for (i = 0; i < count; i++)
    {
      if (str[i] == '\'')
      {
        ptr_size += 3;
        escStr = (char *)realloc(escStr, ptr_size * sizeof(char));
        if (escStr == NULL)
        {
          return NULL;
        }
        sprintf(escStr, "%s'\\''", escStr);
      }
      else
      {
        sprintf(escStr, "%s%c", escStr, str[i]);
      }
    }

    sprintf(escStr, "%s%c", escStr, '\'');
    return escStr;
  }

  std::string exec(const char *cmd)
  {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
      throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
      result += buffer.data();
    }
    return result;
  }
};
