module;

#include <memory>
#include <utility>

module Ostrich;


namespace ostrich
{
    Vm::Vm(Source source, size_t stackSize)
    : m_stackSize{ stackSize }, m_source{ std::move(source) }
    {
    }

    Vm::Vm(const Vm &other)
    : m_stackSize{ other.m_stackSize }, m_stack{ other.m_stack }, m_source{ other.m_source },
      m_cpu(m_stack, m_source, other.m_cpu.nextInstruction(), other.m_cpu.registers())
    {
    }

    void swap(Vm &lhs, Vm &rhs) noexcept
    {
        std::swap(lhs.m_stackSize, rhs.m_stackSize);
        std::swap(lhs.m_stack, rhs.m_stack);
        std::swap(lhs.m_source, rhs.m_source);
        std::swap(lhs.m_cpu, rhs.m_cpu);
    }

    Vm &Vm::operator=(Vm other) noexcept
    {
        swap(*this, other);
        return *this;
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