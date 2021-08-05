#include <iostream>
#include <algorithm>
#include <sstream>
#include "shell/parser.hpp"
#include "shell/shell.hpp"

#define RED_ANSI     "\x1b[91m"
#define GREEN_ANSI   "\x1b[92m"
#define YELLOW_ANSI  "\x1b[93m"
#define BLUE_ANSI    "\u001b[34m"
#define MAGENTA_ANSI "\x1b[95m"
#define RESET_ANSI   "\x1b[0m"


#define BACKGROUND_RED "\u001b[41m"
#define BACKGROUND_GREEN "\u001b[42m"
#define BACKGROUND_YELLOW "\u001b[43m"
#define BACKGROUND_BLUE "\u001b[44m"
#define BACKGROUND_MAGENTA "\u001b[45m"


#define BOLD_ANSI "\u001b[1m"

#define Red(string) RED_ANSI string RESET_ANSI
#define Green(string) GREEN_ANSI string RESET_ANSI
#define Yellow(string) YELLOW_ANSI string RESET_ANSI
#define Blue(string) BLUE_ANSI string RESET_ANSI
#define Magenta(string) MAGENTA_ANSI string RESET_ANSI
#define Black(string) BLACK_ANSI string RESET_ANSI

#define BackgroundRed(string) BACKGROUND_RED string RESET_ANSI
#define BackgroundGreen(string) BACKGROUND_GREEN string RESET_ANSI
#define BackgroundYellow(string) BACKGROUND_YELLOW string RESET_ANSI
#define BackgroundBlue(string) BACKGROUND_BLUE string RESET_ANSI
#define BackgroundMagenta(string) BACKGROUND_MAGENTA string RESET_ANSI


#define Bold(string) BOLD_ANSI string RESET_ANSI

Shell::Shell(const std::string &username):
    In(std::cin),
    Out(std::cout),
    Prompt(BackgroundBlue( + username + "@shell:~$" + ) + Blue("\ue0b0 "))
{
    Register("info",  std::bind(&Shell::Info, this, std::placeholders::_1, std::placeholders::_2));
    Register("exit",  std::bind(&Shell::Exit, this, std::placeholders::_1, std::placeholders::_2));
    Register("clear", std::bind(&Shell::Clear, this, std::placeholders::_1, std::placeholders::_2));
    Register("lc",    std::bind(&Shell::Lc, this, std::placeholders::_1, std::placeholders::_2));
}

void Shell::Register(const std::string &name, CmdDelegate delegate){
    CommandList.insert({name, delegate});
}

int Shell::Interpret(const std::string &line){
    auto statements = Parser::ParseStatements(line);

    for(const auto &s: statements){
        if(!s.size())continue;

        std::vector<Arg> args;
        try{
            args = std::move(Parser::Parse(std::move(s)));
        }catch(ParseError error){
            Out << Red( "[Parse Error]: " + error.Message + ) << std::endl;
            continue;
        }

        try{
            LastCommandCode = ExecuteCommand(args);
        }catch(ExecError error){
            Out << Red("[Error]: " + error.Message + ) << std::endl;
            LastCommandCode = -1;
        }
    }
    return LastCommandCode;
}

int Shell::Run(){
    while(Running){
        PrintPrompt();

        std::string line;
        if(std::getline(In, line).eof()){
            Out << std::endl;
            return LastCommandCode;
        }

        (void)Interpret(line);

        if(LastCommandCode)
            Out << BackgroundBlue( Bold(Red("[x]")));
    }
    return LastCommandCode;
}


void Shell::PrintPrompt(){
    Out << Prompt;
}

int Shell::ExecuteCommand(std::vector<Arg> &args){
    if(!args.size())
        throw ExecError("Zero args command can't be executed");
    if(args[0].Type() != ArgType::Str)
        throw ExecError("Command name should be a String argument");

    auto &name = args[0].As<StrArg>().Data;

    auto cmd = CommandList.find(name);

    if(cmd == CommandList.end())
        throw ExecError("Can't find '" + name + "' command");
    
    return cmd->second(args.size() - 1, args.data() + 1);
}


int Shell::Exit(int argc, Arg argv[]){
    Running = false;
    return 0;
}

int Shell::Clear(int argc, Arg argv[]){
    return system("clear");
}

int Shell::Info(int argc, Arg argv[]){
    std::vector<std::string> commands;
    for(auto pair: CommandList)
        commands.push_back(pair.first);
    std::sort(commands.begin(), commands.end());
    Out << "Shell has " << commands.size() << " commands\n";
    for(auto &c: commands){
        std::cout << "\t" << c << std::endl;
    }
    return 0;
}
    
int Shell::Lc(int argc, Arg argv[]){
    Out << "Last Code: " << LastCommandCode << std::endl;
    return 0;
}