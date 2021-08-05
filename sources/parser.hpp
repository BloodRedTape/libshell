#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include "arg.hpp"
#include "error.hpp"

struct Parser{
    static std::vector<std::string> ParseStatements(const std::string &statement);

    static std::vector<Arg> Parse(std::string statement); 
};

#endif//PARSER_HPP