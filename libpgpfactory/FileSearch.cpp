#include "FileSearch.h"
#include "RnpLoginRequestException.h"
#include <regex>




void FileSearch::searchDown(std::string FolderToSearch,
                            std::string fileRegExStr,
                            const std::vector<std::string> &ignoreSearch,
                            std::string contentRegExStr,
                            std::function<bool(std::string)> contentSearch,
                            std::function<void(std::string)> callback)
{
    RnpLoginRequestException errThreadLoginReq{0, "", "", "", "", "", {}, false};


    const std::regex fileRegEx(fileRegExStr, std::regex_constants::icase);

    for (std::filesystem::recursive_directory_iterator it(FolderToSearch); it != end(it); ++it) {
        auto entry = *it;
        if (entry.is_directory() && isHidden(entry)) {
            it.disable_recursion_pending();
            continue;
        }

        std::string path{entry.path().u8string()};
        std::replace( path.begin(), path.end(), '\\', '/');


        bool isIgnoreSearch = false;
        for (const auto &elem : ignoreSearch) {
            if (path.find(elem) != std::string::npos) {
                isIgnoreSearch = true;
                break;
            }
        }
        if (isIgnoreSearch) {
            it.disable_recursion_pending();
            continue;
        }
        if (entry.is_regular_file() && !isHidden(entry)) {
            std::string relativePath{
                std::filesystem::relative(entry.path(), FolderToSearch).generic_string()};
            std::string relativePathNoExtention{
                std::filesystem::relative(entry.path(), FolderToSearch)
                    .replace_extension()
                    .generic_string()};

            if (std::regex_match(relativePathNoExtention, fileRegEx)) {
                    if (contentSearch(path)) {
                        callback(path);
                    }

            }
        }
    }

}

std::string FileSearch::searchUp(std::string fileOrFolderToSearch,
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

bool FileSearch::isHidden(const std::filesystem::path &p)
{
    std::string name = p.filename().u8string();
    if (name != ".." && name != "." && name[0] == '.') {
        return true;
    }
    return false;
}
