module;

#include <string>
#include <variant>
#include <vector>
export module Ostrich;

namespace ostrich
{
    export enum class RegisterName { rax, rbx, rsp };
    std::string toString(RegisterName registerName);

    export struct Inc
    {
        RegisterName registerName;
        std::string toString() const;
    };

    export struct Dec
    {
        RegisterName registerName;
        std::string toString() const;
    };

    export struct Add
    {
        RegisterName destination;
        RegisterName source;
        std::string toString() const;
    };

    export struct Push
    {
        RegisterName registerName;
        std::string toString() const;
    };

    export struct Mov
    {
        RegisterName destination;
        uint64_t value;
        std::string toString() const;
    };

    export using Instruction = std::variant<Inc, Dec, Add, Push, Mov>;

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

    export using Source = std::vector<Instruction>;

    export class Cpu
    {
    public:
        Cpu(Stack &stack, Source &source);

        void step();
        size_t nextInstruction() const;

        uint64_t rax() const;
        uint64_t rbx() const;
        uint64_t rsp() const;

    private:
        uint64_t &reg(RegisterName r);
        void execute(const Instruction &instruction);

        Stack &m_stack;
        Source &m_source;
        size_t m_nextInstruction{ 0 };
        uint64_t m_rax{ 0 };
        uint64_t m_rbx{ 0 };
        uint64_t m_rsp{ 0 };
    };

    export class Vm
    {
    public:
        Vm(Source source);
        void step();
        const Cpu &cpu() const;
        const Stack &stack() const;
        const Source &source() const;

    private:
        Stack m_stack{ 16, 0xffff };
        Cpu m_cpu{ m_stack, m_source };
        Source m_source;
    };

    export class UI
    {
    public:
        UI(size_t widht, size_t height, Vm &vm);
        void render() const;
        void mainLoop();

    private:
        void render_register(const std::string &name, uint64_t value, char *buf) const;

        size_t m_width;
        size_t m_height;
        Vm &m_vm;
    };
} // namespace ostrich