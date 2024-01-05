#include "RunShellCmd.h"
#include <array>
std::future<std::string> RunShellCmd::runCmdAsync(std::vector<std::string> cmd)
{
    std::future<std::string> result = std::async([=]() { return runCmd(cmd); });
    return result;
}

int RunShellCmd::runSystem(std::vector<std::string> cmd, std::string noEscape)
{
    std::string escapedString{};
    for (const std::string &s : cmd) {
        escapedString = escapedString + escapeshellarg(s) + " ";
    }

    return system((escapedString + noEscape).c_str());
}

bool RunShellCmd::replaceStr(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::string RunShellCmd::exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
#ifdef _WIN32
    std::string s = std::string{"'cmd /c' "} + cmd;
    std::replace(s.begin(), s.end(), '\'', '\"');
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(s.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
#endif
    if (!pipe) {
        std::throw_with_nested(std::runtime_error("popen() failed!"));
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

std::string RunShellCmd::escapeshellarg(std::string str)
{
    std::string ret = str;
    replaceStr(ret, "\\", "\\\\");
    replaceStr(ret, "'", "\\'");
    ret = "'" + str + "'";

    return ret;
}

std::string RunShellCmd::runCmd(std::vector<std::string> cmd, std::string noEscape)
{
    std::string escapedString{};
    for (const std::string &s : cmd) {
        escapedString = escapedString + escapeshellarg(s) + " ";
    }

    return exec((escapedString + noEscape).c_str());
}
