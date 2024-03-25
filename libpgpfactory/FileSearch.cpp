#include "FileSearch.h"
#include "BS_thread_pool.hpp"
#include "RnpLoginRequestException.h"
#include <regex>

void FileSearch::searchDown(std::string FolderToSearch,
                            std::string fileRegExStr,
                            std::string contentRegExStr,
                            std::function<bool(std::string)> contentSearch,
                            std::function<void(std::string)> callback,
                            bool useMultiThread)
{
    bool errorInThread = false;
    std::string errorInThreadMsg;
    RnpLoginRequestException errThreadLoginReq{0, "", "", "", "", "", {}, false};
    BS::thread_pool pool;

    const std::regex fileRegEx(fileRegExStr, std::regex_constants::icase);

    for (std::filesystem::recursive_directory_iterator it(FolderToSearch); it != end(it); ++it) {
        if (errorInThread) {            
            break;
        }
        auto entry = *it;
        if (entry.is_directory() && isHidden(entry)) {
            it.disable_recursion_pending();
            continue;
        }

        if (entry.is_regular_file() && !isHidden(entry)) {
            std::string path{entry.path().u8string()};
            std::string relativePath{
                std::filesystem::relative(entry.path(), FolderToSearch).generic_string()};
            std::string relativePathNoExtention{
                std::filesystem::relative(entry.path(), FolderToSearch)
                    .replace_extension()
                    .generic_string()};
            if (std::regex_match(relativePathNoExtention, fileRegEx)) {
                if (useMultiThread) {
                    pool.detach_task([&errorInThread,
                                      &errorInThreadMsg,
                                      &errThreadLoginReq,
                                      contentSearch,
                                      entry,
                                      callback,
                                      &pool] {
                        try {
                            if (contentSearch(entry.path().u8string())) {
                                callback(entry.path().u8string());
                            }
                        } catch (RnpLoginRequestException &rlre) {                            
                            errorInThread = true;
                            errThreadLoginReq = rlre;
                            pool.purge();
                        } catch (const std::exception &e) {
                            errorInThread = true;
                            errorInThreadMsg = e.what();
                            pool.purge();
                        }
                    });
                } else {
                    if (contentSearch(entry.path().u8string())) {
                        callback(entry.path().u8string());
                    }
                }
            }
        }
    }
    if (useMultiThread) {
        pool.wait();
        if (errorInThread) {            
            if (errThreadLoginReq.code) {
                throw errThreadLoginReq;
            }
            throw std::runtime_error(errorInThreadMsg);
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
