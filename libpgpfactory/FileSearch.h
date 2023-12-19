#pragma once
#include <filesystem>
#include <iostream>
#include <regex>
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
                    std::function<void(std::string s)> callback)
    {
        const std::regex fileRegEx(fileRegExStr, std::regex_constants::icase);

        try {
            for (std::filesystem::recursive_directory_iterator it(FolderToSearch); it != end(it);
                 ++it) {
                auto entry = *it;
                if (entry.is_directory() && isHidden(entry)) {
                    it.disable_recursion_pending();
                    continue;
                }

                if (entry.is_regular_file() && !isHidden(entry)) {
                    std::string path{entry.path().u8string()};
                    std::string relativePath{
                        std::filesystem::relative(entry.path(), FolderToSearch).generic_string()};
                    std::string relativePathNoExtention {
                                                        std::filesystem::relative(entry.path(), FolderToSearch).replace_extension().generic_string()};
                    if (std::regex_match(relativePathNoExtention, fileRegEx) && contentSearch(entry.path().u8string())) {
                        try {
                            callback(entry.path().u8string());
                        } catch (const std::exception &e) {
                            std::throw_with_nested(
                                std::runtime_error(entry.path().string() + ": " + e.what()));
                        }
                    }
                }
            }
        } catch (std::filesystem::filesystem_error &fe) {
        }
    }

    std::string searchUp(std::string fileOrFolderToSearch,
                         std::string currentPath,
                         std::string sstopPath)
    {
        std::filesystem::path pathToDisplay(currentPath);
        std::filesystem::path stopPath(sstopPath);

        if (!std::filesystem::is_directory(pathToDisplay)) {
            pathToDisplay = pathToDisplay.parent_path();
        }

        bool alreadyCheckStopPath = false;
        while (!alreadyCheckStopPath) {
            if (std::filesystem::exists(pathToDisplay.u8string() + std::string("/")
                                        + fileOrFolderToSearch)) {
                return pathToDisplay.u8string();
            }
            if ((pathToDisplay == sstopPath) || (pathToDisplay == pathToDisplay.parent_path())) {
                alreadyCheckStopPath = true;
            }
            pathToDisplay = pathToDisplay.parent_path();
        };

        return "";
    }

    // hygen public
private:
    // hygen private

    bool isHidden(const std::filesystem::path &p)
    {
        std::string name = p.filename().u8string();
        if (name != ".." && name != "." && name[0] == '.') {
            return true;
        }
        return false;
    }
};
