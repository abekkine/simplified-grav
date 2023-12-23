#ifndef CUSTOM_EXCEPTION_HPP
#define CUSTOM_EXCEPTION_HPP

#include <string>
#include <exception>

#include <cstdarg>
#include <cstring>

class CustomException : public std::exception {
private:
    enum {
        max_MESSAGE_BUFFER = 512,
    };
private:
    char message_buffer_[max_MESSAGE_BUFFER];
    std::string message_;
public:
    explicit CustomException(const std::string & message)
    : message_(message) {
        memset(message_buffer_, 0, max_MESSAGE_BUFFER);
    }
    CustomException(const char * format, ...) {
        va_list args;
        va_start(args, format);
        vsnprintf(message_buffer_, max_MESSAGE_BUFFER, format, args);
        va_end(args);
        message_ = std::string(message_buffer_);
    }
    virtual ~CustomException() {}
    virtual const char* what() const throw() {
        return message_.c_str();
    }
};

#endif // CUSTOM_EXCEPTION_HPP
