module;

#include <memory>
#include <utility>

module Ostrich;


namespace ostrich
{
    Vm::Vm(Source source, size_t stackSize) : m_states{ Vm::State{ source, stackSize } }
    {
    }

    void Vm::load(Source source)
    {
        auto stackSize = state().m_stack.content().size();
        m_states.clear();
        m_states.emplace_back(source, stackSize);
    }

    void Vm::step()
    {
        saveState();
        state().m_cpu.step();
    }

    void Vm::execute(const Instruction &instruction)
    {
        saveState();
        state().m_cpu.execute(instruction);
    }

    void Vm::restorePreviousState()
    {
        if(m_states.size() > 1)
        {
            m_states.pop_back();
        }
    }

    const Cpu &Vm::cpu() const
    {
        return state().m_cpu;
    };

    const Stack &Vm::stack() const
    {
        return state().m_stack;
    };

    const Source &Vm::source() const
    {

        return state().m_source;
    }

    Vm::State &Vm::state()
    {
        return m_states.back();
    }

    const Vm::State &Vm::state() const
    {
        return m_states.back();
    }

    void Vm::saveState()
    {
        m_states.push_back(state());
    }

    // State
    Vm::State::State(Source source, size_t stackSize)
    : m_stack{ stackSize, stackTop }, m_source{ std::move(source) }
    {
    }

    Vm::State::State(const Vm::State &other)
    : m_stack{ other.m_stack }, m_source{ other.m_source }, m_cpu{ m_stack, m_source,
                                                                   other.m_cpu.nextInstruction(),
                                                                   other.m_cpu.registers() }
    {
    }

    void swap(Vm::State &lhs, Vm::State &rhs) noexcept
    {
        std::swap(lhs.m_stack, rhs.m_stack);
        std::swap(lhs.m_source, rhs.m_source);
        std::swap(lhs.m_cpu, rhs.m_cpu);
    }

    Vm::State &Vm::State::operator=(Vm::State other) noexcept
    {
        swap(*this, other);
        return *this;
    }
} // namespace ostrich