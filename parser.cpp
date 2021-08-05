#include "parser.hpp"
#include <stack>
#include <iostream>

std::vector<Arg> ParseArgs(std::vector<std::string> args);

template<typename T, size_t N>
size_t lengthof(T (&array)[N]){
    (void)array;
    return N;
}

const char Escape = '\\';
const char Separator = ';';
const char Space = ' ';
const char ListSeparator = ',';
const char DictSeparator = ListSeparator;
const char DictPair = '=';


struct Indentation{
    char Open;
    char Close;

    Indentation(char open, char close):
        Open(open),
        Close(close)
    {}
};

Indentation Indentations[]={
    {'\"', '\"'},
    {'[', ']'},
    {'{', '}'}
};

char GetClosing(char ch){
    for(size_t i = 0; i<sizeof(Indentations)/sizeof(Indentation); i++){
        if(ch == Indentations[i].Open)return Indentations[i].Close;
    }
    assert(false);
    return 0;
}

bool IsOpen(char ch){
    for(auto i: Indentations)
        if(ch == i.Open)return true;
    return false;
}
bool IsClose(char ch){
    for(auto i: Indentations)
        if(ch == i.Close)return true;
    return false;
}

// here escape can protect only Separator character
std::vector<std::string> Parser::ParseStatements(const std::string &line){
    std::vector<std::string> statements;
    for(size_t i = 0; i<line.size(); i++){
        std::string statement;

        for(;i<line.size(); i++){
            if(line[i] == Separator){
                i++;
                break;
            }

            if(line[i] == Escape && line[i + 1] == Separator)
                i++;
            statement.push_back(line[i]);
        }

        statements.push_back(std::move(statement));
    }
    return statements;
}

using ParseProc = Arg (*)(std::string str);

std::string ParseString(std::string str)noexcept{
    std::string arg_str;
    for(size_t i = 0; i<str.size(); ++i){

        if(str[i] == Escape)
            ++i;
        
        arg_str.push_back(str[i]);
    }
    return arg_str;
}

Arg ParseStringArg(std::string str)noexcept{
    return new StrArg(ParseString(std::move(str)));
}

// first list char is '[' and last is ']'
// here Escape protects only ListSeparator
std::vector<Arg> ParseList(std::string list){
    std::string args_list;
    // remove [ ] 
    list = std::string(list.c_str() + 1, list.size() - 2);

    size_t args = 0;

    size_t indentations = 0;
    for(size_t i = 0; i<list.size(); ++i){
        //clear spaces
        for(; i<list.size() && list[i] == Space; ++i);

        if(IsOpen(list[i]))
            indentations++;
        if(IsClose(list[i]))
            indentations--;
        
        if(!indentations){
            if(list[i] == ListSeparator){
                list[i] = Space;
                args++;
            }

            if(list[i] == Escape && list[i + 1] == ListSeparator)
                i++;
        }
        args_list.push_back(list[i]);
    }

    auto parsed_args = Parser::Parse(std::move(args_list));
    if(parsed_args.size() != args + 1)
        throw EmptyObjectError();
    return parsed_args;
}

Arg ParseListArg(std::string list){
    try{
        return new ListArg(ParseList(list));
    }catch(EmptyObjectError error){
        throw ParseError("List can't have empty arguments: " + list);
    }
}

std::string UnparseDict(std::string dict){
    if(dict.size() <= 2)
        throw EmptyObjectError();
    dict = std::string(dict.c_str() + 1, dict.size() - 2);
    std::string unparsed_dict(2, Indentations[(size_t)ArgType::List].Open);

    size_t indentations = 0;
    size_t pairs = 1;
    size_t equals = 0;

    for(size_t i = 0; i<dict.size(); i++){
        if(IsOpen(dict[i]))
            indentations++;
        if(IsClose(dict[i]))
            indentations--;
        
        if(!indentations){
            if(dict[i] == DictSeparator){
                auto &list_indent = Indentations[(size_t)ArgType::List];
                unparsed_dict.push_back(list_indent.Close);
                unparsed_dict.push_back(ListSeparator);
                unparsed_dict.push_back(list_indent.Open);

                pairs++;
                continue;
            }
            if(dict[i] == DictPair){
                unparsed_dict.push_back(ListSeparator);
                if(equals != pairs - 1)
                    throw ParseError("Dictionary can have only Equal sign in pair: " + dict);

                equals++;
                continue;
            }

            if(dict[i] == Escape && (dict[i + 1] == DictSeparator || dict[i + 1] == DictPair))
                i++;
        }
        unparsed_dict.push_back(dict[i]);
    }

    unparsed_dict.append(std::string(2, Indentations[(size_t)ArgType::List].Close));
    return unparsed_dict;
}

std::unordered_map<std::string, Arg> ParseDict(std::string dict){
    auto unparsed_dict = UnparseDict(dict);
    auto args = std::move(Parser::Parse(unparsed_dict)[0].As<ListArg>().Data);

    std::unordered_map<std::string, Arg> map;    

    for(auto &pair: args){
        if(pair.Type() != ArgType::List)
            throw ParseError("Wrong Unparse\n" + dict + "\n" + unparsed_dict);

        auto &list = pair.As<ListArg>();

        if(list.Data.size() != 2)
            throw ParseError("Empty argument at dictionary: " + dict);
        if(list.Data[0].Type()!=ArgType::Str)
            throw ParseError("Dictionary Key should be a String argument: " + dict);
        
        map.emplace(std::move(list.Data[0].As<StrArg>().Data), std::move(list.Data[1]));
    }
    return map;
}

Arg ParseDictArg(std::string dict){
    try{
        return new DictArg(ParseDict(dict));
    }catch(EmptyObjectError error){
        throw ParseError("Dictionary can't have empty arguments: " + dict);
    }
}

ArgType DeclType(const std::string &arg){
    for(size_t i = 1; i<lengthof(Indentations); ++i){
        if(arg[0] == Indentations[i].Open && arg[arg.size() - 1] == Indentations[i].Close)
            return ArgType(i);
    }
    
    for(size_t i = 0; i<arg.size(); ++i){
        for(size_t j = 1; j<lengthof(Indentations); ++j){
            if(arg[i] == Indentations[j].Open || arg[i] == Indentations[j].Close)
                throw ParseError("Can't determine argument type: " + arg);
        }

        if(arg[i] == Escape)
            ++i;
    }
    return ArgType::Str;
}

ParseProc ArgParseProcs[] = {
    ParseStringArg,
    ParseListArg,
    ParseDictArg
};

// args don't have spaces to remove  // Arg string can't be zero sized
std::vector<Arg> ParseArgs(std::vector<std::string> args){
    std::vector<Arg> parsed;
    for(auto &s: args){
        ArgType type = DeclType(s);
        parsed.emplace_back(ArgParseProcs[(size_t)type](std::move(s)));
    }
    return parsed;
}

std::vector<Arg> Parser::Parse(std::string statement){
    std::vector<std::string> args;

    for(size_t i = 0; i<statement.size(); ++i){
        //clear spaces;
        for(;i<statement.size() && statement[i] == Space; i++);

        std::string arg;

        std::stack<char> identations;

        for(;i<statement.size(); i++){
            if(statement[i] == Space && !identations.size())break;

            if(IsOpen(statement[i]))
                identations.push(statement[i]);
            if(IsClose(statement[i])){
                if(!identations.size())
                    throw ParseError("Statement closing nested to much");
                char top = identations.top();
                identations.pop();

                if(GetClosing(top) != statement[i])
                    throw ParseError("Statement has wrong closing indentations: " + arg);
            }

            if(statement[i] == Escape)
                arg.push_back(statement[i++]);

            arg.push_back(statement[i]);
        }

        if(identations.size())
            throw ParseError("Statement opening nested to much: " + arg);

        args.push_back(arg);
    }

    return ParseArgs(std::move(args));
}