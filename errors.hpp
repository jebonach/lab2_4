#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

enum class ErrorType {
    OutOfRange,
    InvalidArg,
    NegativeSize,
    Unknown
};

class MyException {
    ErrorType type_;
    uint8_t code_;
public:
    MyException(ErrorType t, uint8_t c)
        : type_(t), code_(c) {}

    ErrorType getType() const { return type_; }
    uint8_t getCode() const { return code_; }

    const char* what() const { return "MyException"; }
};

struct ErrorInfo { int code; std::string message; };

inline std::vector<ErrorInfo> g_ErrorTable = {
    {0,  "Menu input not an integer"},
    {1,  "Value input not an integer"},
    {2,  "Negative size / degree = 0"},
    {3,  "Index out of range (>= size)"},
    {4,  "Start index > end index"},
    {5,  "Tree is empty"},
    {6,  "Root already exists"},
    {7,  "Node already exists"},
    {8,  "Invalid path"},
    {9,  "Tree id out of range"},
    {10, "No trees were created yet"}
};

inline std::string getErrorMessage(int code)
{
    for (auto& e : g_ErrorTable)
        if (e.code == code) {
            return e.message;
        }
    return "[Unknown error code]";
}

inline void handleException(const MyException& ex)
{
    switch (ex.getType()) {
        case ErrorType::OutOfRange:std::cout << "[OutOfRange] ";
            break;
        case ErrorType::InvalidArg:std::cout << "[InvalidArg] ";
            break;
        case ErrorType::NegativeSize:std::cout << "[NegativeSize] ";
            break;
        default:std::cout << "[Unknown] ";
            break;
    }
    std::cout << "code=" << int(ex.getCode())
              << " => " << getErrorMessage(ex.getCode()) << '\n';
}
