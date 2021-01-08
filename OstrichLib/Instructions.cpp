module;

#include "Overloaded.h"

#include <fmt/core.h>

#include <string>
#include <variant>

module Ostrich;

namespace ostrich
{
    std::string toString(uint64_t value)
    {
        return fmt::format("0x{0:X}", value);
    }

    std::string toString(RegisterOrImmediateOrMemory r)
    {
        return std::visit(overloaded{
                          [](const RegisterName name) { return ostrich::toString(name); },
                          [](const uint64_t value) { return ostrich::toString(value); },
                          [](const MemoryAddress &value) { return fmt::format("qword ptr [{0}]",value.toString()); },
                          },
                          r);
    }

    std::string Inc::toString() const
    {
        return "inc  " + ostrich::toString(registerName);
    }

    std::string Dec::toString() const
    {
        return "dec  " + ostrich::toString(registerName);
    }

    std::string Add::toString() const
    {
        return "add  " + ostrich::toString(destination) + " " + ostrich::toString(source);
    }

    std::string Push::toString() const
    {
        return "push " + ostrich::toString(registerName);
    }

    std::string Pop::toString() const
    {
        return "pop  " + ostrich::toString(registerName);
    }

    std::string Mov::toString() const
    {
        return "mov  " + ostrich::toString(destination) + " " + ostrich::toString(source);
    }

} // namespace ostrich
