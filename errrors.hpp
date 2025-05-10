#pragma once
#include <vector>
#include <string>
#include "NewExceptions.h"
#include <exception>
#include <cstdint>

enum class ErrorType {
    OutOfRange,
    InvalidArg,
    NegativeSize,
    Unknown
};

class MyException : public std::exception {
private:
    ErrorType type;
    uint8_t code;

public:
    MyException(ErrorType t, uint8_t c)
        : type(t), code(c) {}

    ErrorType getType() const noexcept {
        return type;
    }

    uint8_t getCode() const noexcept {
        return code;
    }

    const char* what() const noexcept override {
        return "MyException (type+code)";
    }
};

struct ErrorInfo {
    int code;
    std::string message;
};

extern std::vector<ErrorInfo> g_ErrorTable; 

void handleException(const MyException &ex);
