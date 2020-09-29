module;

#include <fmt/core.h>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
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
        case rsi:
            return "rsi";
        case rdi:
            return "rdi";
        case rbp:
            return "rbp";
        case rsp:
            return "rsp";
        }
        static_assert(registerCount == 8, "Don't forget to update this!");
        throw std::runtime_error("No such register");
    }

    std::ostream &operator<<(std::ostream &os, RegisterName registerName)
    {
        os << toString(registerName);
        return os;
    }
} // namespace ostrich