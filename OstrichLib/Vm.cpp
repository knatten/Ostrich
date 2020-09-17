module;

#include <memory>

module Ostrich;


namespace ostrich
{
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