#ifndef SHELL_HPP
#define SHELL_HPP

#include <unordered_map>
#include <string>
#include "command.hpp"

class Shell{
private:
    std::unordered_map<std::string, CmdDelegate> CommandList;
    std::istream &In;
    std::ostream &Out;
    std::string Prompt;

    int LastCommandCode = 0;
    bool Running = true;
public:
    Shell(const std::string &username);

    void Register(const std::string &name, CmdDelegate delegate);

    int Interpret(const std::string &line);

    int Run();
private:
    void PrintPrompt();

    int ExecuteCommand(std::vector<Arg> &args);

    int Exit(int argc, Arg argv[]);

    int Clear(int argc, Arg argv[]);

    int Info(int argc, Arg argv[]);

    int Lc(int argc, Arg argv[]);

};

#endif//SHELL_HPP