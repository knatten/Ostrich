module;

#include <stdexcept>
#include <string>
module Ostrich;


namespace ostrich
{
    std::string toString(RegisterName registerName)
    {
        switch(registerName)
        {
            using enum RegisterName;
        case rax:
            return "rax";
        case rbx:
            return "rbx";
        case rsp:
            return "rsp";
        }
        throw std::runtime_error("No such register");
    }
} // namespace ostrich