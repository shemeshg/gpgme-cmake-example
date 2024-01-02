#pragma once
#include <filesystem>
#include <string>
#include <functional>

class FileSearch
{
public:
    FileSearch() = default;

    FileSearch(FileSearch const &) = delete;
    FileSearch &operator=(FileSearch const &) = delete;
    FileSearch(FileSearch &&) = delete;
    FileSearch &operator=(FileSearch &&) = delete;

    ~FileSearch(){};

    void searchDown(std::string FolderToSearch,
                    std::string fileRegExStr,
                    std::string contentRegExStr,
                    std::function<bool(std::string s)> contentSearch,
                    std::function<void(std::string s)> callback,
                    bool useMultiThread);

    std::string searchUp(std::string fileOrFolderToSearch,
                         std::string currentPath,
                         std::string sstopPath);

    // hygen public
private:
    // hygen private

    bool isHidden(const std::filesystem::path &p);
};
