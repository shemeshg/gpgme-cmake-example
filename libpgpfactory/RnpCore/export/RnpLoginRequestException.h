#include <exception>
#include <string>

struct RnpLoginRequestException : public std::exception
{
    int code;
    std::string message = "";
    std::string functionName;
    std::string lastKeyIdRequested;
    std::string filePath;

    RnpLoginRequestException(int code,
                             std::string message,
                             std::string functionName,
                             std::string lastKeyIdRequested,
                             std::string filePath)
        : code(code)
        , message(message)
        , functionName(functionName)
        , lastKeyIdRequested{lastKeyIdRequested}
        , filePath{filePath}
    {}

    const char *what() const noexcept override { return message.c_str(); }
};
