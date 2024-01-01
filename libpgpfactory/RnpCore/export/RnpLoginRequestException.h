#include <exception>
#include <string>

struct RnpLoginRequestException : public std::exception
{
    int code;
    std::string message = "";
    std::string functionName;
    std::string lastKeyIdRequested;
    std::string fromFilePath;
    std::string toFilePath;
    std::vector<std::string> encryptTo;
    bool doSign;

    RnpLoginRequestException(int code,
                             std::string message,
                             std::string functionName,
                             std::string lastKeyIdRequested,
                             std::string fromFilePath,
                             std::string toFilePath,
                             std::vector<std::string> encryptTo,
                                     bool doSign)
        : code(code)
        , message(message)
        , functionName(functionName)
        , lastKeyIdRequested{lastKeyIdRequested}
        , fromFilePath{fromFilePath}
        , toFilePath{toFilePath}
        , encryptTo{encryptTo}
        , doSign{doSign}
    {}

    const char *what() const noexcept override { return message.c_str(); }
};
