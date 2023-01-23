#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <future>
#include <filesystem>
#include <random>
#include <sstream>

namespace uuid
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static std::uniform_int_distribution<> dis2(8, 11);

  std::string generate_uuid_v4()
  {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++)
    {
      ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++)
    {
      ss << dis(gen);
    };
    return ss.str();
  }
}

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
      escapedString = escapedString + escapeshellarg(s) + " ";
    }

    return exec(escapedString.c_str());
  }

private:
  bool replaceStr(std::string &str, const std::string &from, const std::string &to)
  {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
      return false;
    str.replace(start_pos, from.length(), to);
    return true;
  }

  std::string escapeshellarg(std::string str)
  {
    std::string ret = str;
    replaceStr(ret, "\\", "\\\\");
    replaceStr(ret, "'", "\\'");
    ret = "'" + str + "'";

    return ret;
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

class TmpFileWacher
{
public:
  TmpFileWacher(std::string fileName) : m_fileName{fileName}
  {
    createTmpFolder();
  }

  void init(std::string tmpFolder = ""){
    if(!tmpFolder.empty()){m_tmpFolder = tmpFolder;}
    createTmpFolder();
  }

  std::filesystem::path getSubfolderPath()
  {
    return m_tmpFolder / m_tmpSubfolder;
  }

  std::filesystem::path getFullFilePath()
  {
    return m_tmpFolder / m_tmpSubfolder / m_fileName;
  }  

  ~TmpFileWacher()
  {
    deleteTmpFolder();
  }

private:
  std::filesystem::path m_tmpFolder{std::filesystem::temp_directory_path()};
  std::string m_tmpSubfolder{uuid::generate_uuid_v4()};
  std::string m_fileName{};

  void createTmpFolder()
  {
    if (std::filesystem::exists(m_tmpFolder)){
      std::filesystem::create_directory(getSubfolderPath());
    }  else {
      std::throw_with_nested(std::runtime_error("Temp folder not found"));
    }      
  }

  void deleteTmpFolder()
  {
    std::filesystem::remove_all(getSubfolderPath());
  }
};