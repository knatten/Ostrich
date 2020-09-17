module;

#include <stdexcept>
#include <variant>
module Ostrich;


namespace ostrich
{
    template <class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    Cpu::Cpu(Stack &stack, Source &source) : m_stack(&stack), m_source(&source)
    {
        reg(RegisterName::rsp) = stack.top();
    }

    void Cpu::step()
    {
        if(m_nextInstruction == m_source->size())
        {
            return;
        }
        execute((*m_source)[m_nextInstruction]);
        m_nextInstruction++;
    }

    void Cpu::execute(const Instruction &instruction)
    {
        std::visit(overloaded{
                   [this](const Inc &inc) { reg(inc.registerName)++; },
                   [this](const Dec &dec) { reg(dec.registerName)--; },
                   [this](const Add &add) { reg(add.destination) += reg(add.source); },
                   [this](const Push &push) {
                       m_stack->store(reg(RegisterName::rsp), reg(push.registerName));
                       reg(RegisterName::rsp) -= 8;
                   },
                   [this](const Pop &pop) {
                       reg(pop.registerName) = m_stack->load(reg(RegisterName::rsp) + 8);
                       reg(RegisterName::rsp) += 8;
                   },
                   [this](const Mov &mov) { reg(mov.destination) = mov.value; },
                   },
                   instruction);
    }

    size_t Cpu::nextInstruction() const
    {
        return m_nextInstruction;
    }
    const std::array<Register, 3> Cpu::registers() const
    {
        return m_registers;
    }

    const uint64_t &Cpu::reg(RegisterName r) const
    {
        return std::find_if(m_registers.begin(), m_registers.end(),
                            [&](const auto reg) { return reg.registerName == r; })
        ->value;
        throw std::runtime_error("No such register");
    }

    uint64_t &Cpu::reg(RegisterName r)
    {
        return const_cast<uint64_t &>(const_cast<const Cpu &>(*this).reg(r));
    }

    void Vm::step()
    {
        m_cpu.step();
    }

    const Cpu &Vm::cpu() const
    {
        return m_cpu;
    };

} // namespace ostrich
