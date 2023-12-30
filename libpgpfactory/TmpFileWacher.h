#pragma once
#include <filesystem>
#include "uuid.h"

class TmpFileWacher
{
public:
  TmpFileWacher() {}

  void init(std::string fileName, std::string tmpFolder = "")
  {
    m_fileName = fileName;
    if (!tmpFolder.empty())
    {
      m_tmpFolder = tmpFolder;
    }
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
    if (std::filesystem::exists(m_tmpFolder))
    {
      std::filesystem::create_directory(getSubfolderPath());
    }
    else
    {
      std::throw_with_nested(std::runtime_error("Temp folder not found"));
    }
  }

  void deleteTmpFolder()
  {
    std::filesystem::remove_all(getSubfolderPath());
  }
};
