export module Ostrich;

import std.core;

namespace ostrich
{
    export enum class RegisterName { rax, rbx, rsp };

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

    export struct Push
    {
        RegisterName registerName;
    };

    export using Instruction = std::variant<Inc, Dec, Add, Push>;

    export class Stack
    {
    public:
        Stack(uint64_t size, uint64_t top);

        const std::vector<uint8_t> content() const;
        uint64_t top() const;
        void store(uint64_t address, uint64_t value);

    private:
        uint64_t m_size;
        uint64_t m_top;
        std::vector<uint8_t> m_content;
    };

    export class Cpu
    {
    public:
        Cpu(Stack &stack);

        void execute(const Instruction &instruction);
        uint64_t rax() const;
        uint64_t rbx() const;
        uint64_t rsp() const;

    private:
        uint64_t &reg(RegisterName r);

        Stack &m_stack;
        uint64_t m_rax{ 0 };
        uint64_t m_rbx{ 0 };
        uint64_t m_rsp{ 0 };
    };

    export class Vm
    {
    public:
        const Cpu &cpu() const;
        Cpu &cpu(); // TODO don't expose this directly
        const Stack &stack() const;

    private:
        Stack m_stack{ 16, 0xffff };
        Cpu m_cpu{ m_stack };
    };

    export std::string visualize(const Vm &vm);
} // namespace ostrich