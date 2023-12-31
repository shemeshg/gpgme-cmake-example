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

    RnpLoginRequestException(int code,
                             std::string message,
                             std::string functionName,
                             std::string lastKeyIdRequested,
                             std::string fromFilePath,
                             std::string toFilePath)
        : code(code)
        , message(message)
        , functionName(functionName)
        , lastKeyIdRequested{lastKeyIdRequested}
        , fromFilePath{fromFilePath}
        , toFilePath{toFilePath}
    {}

    const char *what() const noexcept override { return message.c_str(); }
};
