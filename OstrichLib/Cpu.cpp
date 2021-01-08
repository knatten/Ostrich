module;

#include "Overloaded.h"

#include <array>
#include <stdexcept>
#include <variant>
module Ostrich;


namespace ostrich
{

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
                       registerValue(add.destination) += readValue(add.source);
                   },
                   [this](const Push &push) {
                       m_stack->store(registerValue(RegisterName::rsp), registerValue(push.registerName));
                       registerValue(RegisterName::rsp) -= 8;
                   },
                   [this](const Pop &pop) {
                       registerValue(pop.registerName) = m_stack->load(registerValue(RegisterName::rsp) + 8);
                       registerValue(RegisterName::rsp) += 8;
                   },
                   [this](const Mov &mov) { registerValue(mov.destination) = readValue(mov.source); },
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

    uint64_t Cpu::loadEffectiveAddress(const MemoryAddress &address) const
    {
        using enum AdditiveOperator;
        uint64_t result = registerValue(address.base);
        if(address.index)
        {
            switch(address.indexOperator)
            {
            case plus:
                result += registerValue(*(address.index)) * address.scale;
                break;
            case minus:
                result -= registerValue(*(address.index)) * address.scale;
                break;
            }
        }
        switch(address.displacementOperator)
        {
        case plus:
            result += address.displacement;
            break;
        case minus:
            result -= address.displacement;
            break;
        }
        return result;
    }

    uint64_t Cpu::memoryValue(const MemoryAddress &address) const
    {
        return m_stack->load(loadEffectiveAddress(address));
    }

    uint64_t Cpu::readValue(RegisterOrImmediateOrMemory r)
    {
        return std::visit(overloaded{
                          [this](const RegisterName name) { return registerValue(name); },
                          [this](const uint64_t value) { return value; },
                          [this](const MemoryAddress &address) { return memoryValue(address); },
                          },
                          r);
    }

} // namespace ostrich
