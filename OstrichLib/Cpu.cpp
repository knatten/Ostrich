module;

#include <stdexcept>
#include <variant>
module Ostrich;


namespace ostrich
{
    template <class... Ts> struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    Cpu::Cpu(Stack &stack, std::vector<Instruction> &source)
    : m_stack(stack), m_source(source), m_rsp{ stack.top() }
    {
    }

    void Cpu::step()
    {
        execute(m_source[m_nextInstruction]);
        m_nextInstruction++;
    }

    void Cpu::execute(const Instruction &instruction)
    {
        std::visit(overloaded{
                   [this](const Inc &inc) { reg(inc.registerName)++; },
                   [this](const Dec &dec) { reg(dec.registerName)--; },
                   [this](const Add &add) { reg(add.destination) += reg(add.source); },
                   [this](const Push &push) {
                       m_stack.store(m_rsp, reg(push.registerName));
                       m_rsp -= 8;
                   },
                   },
                   instruction);
    }

    size_t Cpu::nextInstruction() const
    {
        return m_nextInstruction;
    }

    uint64_t Cpu::rax() const
    {
        return m_rax;
    }

    uint64_t Cpu::rbx() const
    {
        return m_rbx;
    }

    uint64_t Cpu::rsp() const
    {
        return m_rsp;
    }

    uint64_t &Cpu::reg(RegisterName r)
    {
        switch(r)
        {
            using enum RegisterName;
        case rax:
            return m_rax;
        case rbx:
            return m_rbx;
        case rsp:
            return m_rsp;
        }
        throw std::runtime_error("No such register");
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
