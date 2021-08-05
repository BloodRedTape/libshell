#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "shell/arg.hpp"
#include <functional>

using CmdDelegate = std::function<int(int, Arg[])>;

#endif//COMMAND_HPP