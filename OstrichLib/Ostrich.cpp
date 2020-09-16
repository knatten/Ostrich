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

    Vm::Vm(Source source) : m_source{ std::move(source) }
    {
    }

    const Stack &Vm::stack() const
    {
        return m_stack;
    };

    const Source &Vm::source() const
    {

        return m_source;
    }
} // namespace ostrich