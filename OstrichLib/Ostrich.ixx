export module Ostrich;

import std.core;

namespace ostrich
{
    export class Inc
    {
    };

    export class Dec
    {
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
            std::visit(overloaded{ [this](const Inc &inc) { m_eax++; },
                                   [this](const Dec &dec) { m_eax--; } },
                       instruction);
        }
        uint64_t eax()
        {
            return m_eax;
        }

    private:
        uint64_t m_eax{ 0 };
    };
} // namespace ostrich