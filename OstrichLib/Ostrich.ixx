export module Ostrich;

import std.core;

namespace ostrich
{
    export enum class RegisterName
    {
        rax, rbx
    };

    export struct Inc
    {
        RegisterName registerName;
    };

    export struct Dec
    {
        RegisterName registerName;
    };

    export using Instruction = std::variant<Inc, Dec>;

    template <class... Ts> struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    export class Cpu
    {
    public:
        void execute(Instruction instruction)
        {
            std::visit(overloaded{ [this](const Inc &inc) { reg(inc.registerName)++; },
                                   [this](const Dec &dec) { reg(dec.registerName)--; } },
                       instruction);
        }
        uint64_t rax() const
        {
            return m_rax;
        }
        uint64_t rbx() const
        {
            return m_rbx;
        }

    private:
        uint64_t &reg(RegisterName r)
        {
            switch(r)
            {
                using enum RegisterName;
            case rax:
                return m_rax;
            case rbx:
                return m_rbx;
            }
            throw std::runtime_error("No such register");
        }
        uint64_t m_rax{ 0 };
        uint64_t m_rbx{ 0 };
    };
} // namespace ostrich