#ifndef ARG_HPP
#define ARG_HPP

#include <string>
#include <vector>
#include <assert.h>
#include <unordered_map>
#include <sstream>

enum class ArgType{
    Str = 0,
    List = 1,
    Dict = 2
};

#define ArgMetaData(arg_type) \
    ArgType Type()const override{\
        return StaticType();\
    }\
    static ArgType StaticType(){\
        return arg_type;\
    }


struct ArgBase{
    virtual ~ArgBase(){};

    virtual ArgType Type()const = 0;
};

class Arg{
private:
    ArgBase *m_Base = nullptr;
public:
    Arg() = default;

    Arg(ArgBase *base):
        m_Base(base)
    {}

    Arg(Arg&& other)noexcept{
        m_Base = other.m_Base;
        other.m_Base = nullptr;
    }

    ~Arg(){
        delete m_Base;
    }

    ArgType Type()const{
        assert(m_Base);
        return m_Base->Type();
    }

    template<typename T>
    T &As(){
        assert(m_Base->Type() == T::StaticType());
        return *static_cast<T*>(m_Base);
    }

    template<typename T>
    T &As()const{
        return const_cast<Arg*>(this)->As<T>();
    }

};


struct StrArg: ArgBase{
    ArgMetaData(ArgType::Str)

    std::string Data;

    StrArg(std::string data):
        Data(std::move(data)) 
    {}

    template <typename T>
    bool TryParse(T &out){
        std::stringstream stream(Data);
        stream >> out;
        char ch;
        return !stream.fail() && (stream >> ch).eof();
    }
};

struct ListArg: ArgBase{
    ArgMetaData(ArgType::List)

    std::vector<Arg> Data;

    ListArg(std::vector<Arg> data):
        Data(std::move(data)) 
    {}
};


struct DictArg: ArgBase{
    ArgMetaData(ArgType::Dict)

    std::unordered_map<std::string, Arg> Data;

    DictArg(std::unordered_map<std::string, Arg> data):
        Data(std::move(data)) 
    {}

};

#endif//ARG_HPP