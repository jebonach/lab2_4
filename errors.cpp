#include "errors.h"
#include <iostream>

std::vector<ErrorInfo> g_ErrorTable = {
    {0, "Menu input not an integer?"},
    {1, "Array input was not integer?"},
    {2, "Index < 0?"},
    {3, "Index >= size?"},
    {4, "start > end?"},
    {5, "Empty?"},
    {6, "Unknown sub-code?"},
};

static std::string getErrorMessage(int code) {
    for (auto &err : g_ErrorTable) {
        if (err.code == code) {
            return err.message;
        }
    }
    return "[No known message for code]";
}

void handleException(const MyException &ex) {
    switch (ex.getType()) {
    case ErrorType::OutOfRange:
        std::cout << "[OutOfRange] "; 
        break;
    case ErrorType::InvalidArg:
        std::cout << "[InvalidArg] ";
        break;
    case ErrorType::NegativeSize:
        std::cout << "[NegativeSize] ";
        break;
    default:
        std::cout << "[Unknown type] ";
        break;
    }

    std::cout << "code=" << (int)ex.getCode() << " => " 
              << getErrorMessage(ex.getCode()) << "\n";
}
