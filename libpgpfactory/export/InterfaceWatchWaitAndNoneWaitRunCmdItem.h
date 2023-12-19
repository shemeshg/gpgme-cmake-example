#pragma once
#include <string>
#include <filesystem>

class InterfaceWatchWaitAndNoneWaitRunCmdItem{
    public:
    virtual ~InterfaceWatchWaitAndNoneWaitRunCmdItem(){}
    virtual const  std::string getUniqueId() const =0;
    virtual std::filesystem::path getFullFilePath() = 0;
    virtual std::filesystem::path getSubfolderPath() = 0;
    virtual  void init() = 0;
    virtual void runWithWait() = 0;
};