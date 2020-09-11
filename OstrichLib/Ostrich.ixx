export module Ostrich;

import std.core;

namespace ostrich
{
    export enum class RegisterName { rax, rbx };

    export struct Inc
    {
        RegisterName registerName;
    };

    export struct Dec
    {
        RegisterName registerName;
    };

    export struct Add
    {
        RegisterName destination;
        RegisterName source;
    };

    export using Instruction = std::variant<Inc, Dec, Add>;

    export class Cpu
    {
    public:
        void execute(const Instruction &instruction);
        uint64_t rax() const;
        uint64_t rbx() const;

    private:
        uint64_t &reg(RegisterName r);

        uint64_t m_rax{ 0 };
        uint64_t m_rbx{ 0 };
    };
} // namespace ostrich