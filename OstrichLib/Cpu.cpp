module;

#include <array>
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
        for(size_t i = 0; i < registerCount; ++i)
        {
            m_registers[i] = Register{ static_cast<RegisterName>(i), 0 };
        }
        registerValue(RegisterName::rsp) = stack.beginning();
    }

    Cpu::Cpu(Stack &stack, Source &source, size_t nextInstruction, std::array<Register, registerCount> registers)
    : m_stack{ &stack }, m_source{ &source }, m_nextInstruction{ nextInstruction }, m_registers{ std::move(registers) }
    {
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
                   [this](const Inc &inc) { registerValue(inc.registerName)++; },
                   [this](const Dec &dec) { registerValue(dec.registerName)--; },
                   [this](const Add &add) {
                       registerValue(add.destination) += registerValue(add.source);
                   },
                   [this](const Push &push) {
                       m_stack->store(registerValue(RegisterName::rsp), registerValue(push.registerName));
                       registerValue(RegisterName::rsp) -= 8;
                   },
                   [this](const Pop &pop) {
                       registerValue(pop.registerName) = m_stack->load(registerValue(RegisterName::rsp) + 8);
                       registerValue(RegisterName::rsp) += 8;
                   },
                   [this](const Mov &mov) { registerValue(mov.destination) = mov.value; },
                   },
                   instruction);
    }

    size_t Cpu::nextInstruction() const
    {
        return m_nextInstruction;
    }
    const std::array<Register, registerCount> Cpu::registers() const
    {
        return m_registers;
    }

    const uint64_t &Cpu::registerValue(RegisterName r) const
    {
        return std::find_if(m_registers.begin(), m_registers.end(),
                            [&](const auto reg) { return reg.registerName == r; })
        ->value;
        throw std::runtime_error("No such register");
    }

    uint64_t &Cpu::registerValue(RegisterName r)
    {
        return const_cast<uint64_t &>(const_cast<const Cpu &>(*this).registerValue(r));
    }

} // namespace ostrich
