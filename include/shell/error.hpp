#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

struct ParseError{
    std::string Message;

    ParseError(std::string msg):
        Message(std::move(msg))
    {}
};

struct EmptyObjectError{ };

struct ExecError{
    std::string Message;

    ExecError(std::string msg):
        Message(std::move(msg))
    {}
};


#endif