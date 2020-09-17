module;

#include <memory>

module Ostrich;


namespace ostrich
{
    Vm::Vm(Source source) : m_source{ std::move(source) }
    {
    }

    void Vm::load(Source source)
    {
        m_stack = Stack{ stackSize, stackTop };
        m_source = std::move(source);
        m_cpu = Cpu{ m_stack, m_source };
    }

    void Vm::execute(const Instruction &instruction)
    {
        m_cpu.execute(instruction);
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