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
        case rcx:
            return "rcx";
        case rdx:
            return "rdx";
        case rsp:
            return "rsp";
        }
        static_assert(registerCount == 5, "Don't forget to update this!");
        throw std::runtime_error("No such register");
    }
} // namespace ostrich