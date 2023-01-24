#pragma once
#include <exception>
#include <stdexcept>
#include <string_view>

class StandardException :public std::runtime_error {
public:
    StandardException(const std::string& s):std::runtime_error(s){};
    virtual ~StandardException() = default;
    virtual void setMessage(std::string_view msg)final {
        this->message = std::move(msg);
    }
    virtual const std::string& getMessage()const final {
        return this->message;
    }
private:
    std::string message{ "Serious Error!!!" };
};