module;

#include <memory>

module Ostrich;


namespace ostrich
{
    Vm::Vm(Source source, size_t stackSize)
    : m_source{ std::move(source) }, m_stackSize{ stackSize }
    {
    }

    void Vm::load(Source source)
    {
        m_stack = Stack{ m_stackSize, stackTop };
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