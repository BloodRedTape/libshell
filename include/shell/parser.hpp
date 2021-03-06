#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include "shell/arg.hpp"
#include "shell/error.hpp"

struct Parser{
    static std::vector<std::string> ParseStatements(const std::string &statement);

    static std::vector<Arg> Parse(std::string statement); 
};

#endif//PARSER_HPP