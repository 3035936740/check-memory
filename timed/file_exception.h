#pragma once
#include <string_view>
#include "standard_exception.h"

class FileException : public StandardException {
public:
    FileException() = default;
    FileException(const std::string& msg) :StandardException(msg) {};

    ~FileException()final = default;
private:
};