#pragma once

#include <string>
#include <vector>
#include <future>

class RunShellCmd
{
public:
  RunShellCmd() {}
  std::future<std::string> runCmdAsync(std::vector<std::string> cmd);

  int runSystem(std::vector<std::string> cmd, std::string noEscape = "");

  std::string runCmd(std::vector<std::string> cmd, std::string noEscape = "");

  private:
  bool replaceStr(std::string &str, const std::string &from, const std::string &to);

  std::string escapeshellarg(std::string str);

  virtual std::string exec(const char *cmd);
};





