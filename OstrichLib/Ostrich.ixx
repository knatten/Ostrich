module;

#include <array>
#include <filesystem>
#include <ostream>
#include <string>
#include <variant>
#include <vector>
export module Ostrich;

namespace ostrich
{
    // Registers
    export enum class RegisterName { rax, rbx, rsp };
    std::string toString(RegisterName registerName);

    export struct Register
    {
        RegisterName registerName;
        uint64_t value;
    };

    // Instructions
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

    export struct Pop
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

    export using Instruction = std::variant<Inc, Dec, Add, Push, Pop, Mov>;
    export using Source = std::vector<Instruction>;

    export template <typename InstructionType>
    concept InstructionSingleRegisterOperand =
    std::is_same_v<InstructionType, Inc> || std::is_same_v<InstructionType, Dec> ||
    std::is_same_v<InstructionType, Push> || std::is_same_v<InstructionType, Pop>;

    export template <typename InstructionType>
    concept InstructionAny = InstructionSingleRegisterOperand<InstructionType> ||
                             std::is_same_v<InstructionType, Add> || std::is_same_v<InstructionType, Mov>;

    export template <InstructionAny LhsInstruction, InstructionAny RhsInstruction>
    bool operator==(const LhsInstruction &lhs, const RhsInstruction &rhs)
    {
        return false;
    }

    export template <InstructionSingleRegisterOperand LhsInstruction, InstructionSingleRegisterOperand RhsInstruction>
    bool operator==(const LhsInstruction &lhs, const RhsInstruction &rhs)
    {
        return std::is_same_v<LhsInstruction, RhsInstruction> && lhs.registerName == rhs.registerName;
    }

    export bool operator==(const Add &lhs, const Add &rhs)
    {
        return lhs.destination == rhs.destination && lhs.source == rhs.source;
    }

    export bool operator==(const Mov &lhs, const Mov &rhs)
    {
        return lhs.destination == rhs.destination && lhs.value == rhs.value;
    }

    export template <InstructionAny I>
    std::ostream &operator<<(std::ostream &os, I instruction)
    {
        os << instruction.toString();
        return os;
    }

    export std::ostream &operator<<(std::ostream &os, Instruction instruction)
    {
        os << std::visit([](const auto &i) { return i.toString(); }, instruction);
        return os;
    }

    // Stack
    export class Stack
    {
    public:
        Stack(uint64_t size, uint64_t top);

        const std::vector<uint8_t> content() const;
        uint64_t top() const;
        void store(uint64_t address, uint64_t value);
        uint64_t load(uint64_t addres);

    private:
        uint64_t m_size;
        uint64_t m_top;
        std::vector<uint8_t> m_content;
    };

    constexpr std::array<Register, 3> initRegisters()
    {
        using enum RegisterName;
        // TODO niceify with ranges or something?
        return { Register{ rax, 0 }, Register{ rbx, 0 }, Register{ rsp, 0 } };
    }

    // Cpu
    export class Cpu
    {
    public:
        Cpu(Stack &stack, Source &source);

        void step();
        void execute(const Instruction &instruction);
        size_t nextInstruction() const;
        const std::array<Register, 3> registers() const;

        const uint64_t &reg(RegisterName r) const; //TODO rename this and other overload to registerValue

    private:
        uint64_t &reg(RegisterName r);

        Stack *m_stack;
        Source *m_source;
        size_t m_nextInstruction{ 0 };
        std::array<Register,3> m_registers{ initRegisters() };
    };

    // Vm
    export class Vm
    {
    public:
        Vm(Source source, size_t stackSize);

        void load(Source source);
        void step();
        void execute(const Instruction &instruction);
        const Cpu &cpu() const;
        const Stack &stack() const;
        const Source &source() const;

    private:
        uint64_t m_stackSize;
        static constexpr uint64_t stackTop{ 0xffff };
        Stack m_stack{ m_stackSize, stackTop };
        Source m_source;
        Cpu m_cpu{ m_stack, m_source };
    };

    // Parser
    export Instruction parseInstruction(const std::string_view &sourceLine);
    export Source parse(const std::string_view &sourceText);
    export Source parse(const std::filesystem::path &sourcePath);
    // split_view is not implemented yet, so I stole https://www.bfilipek.com/2018/07/string-view-perf-followup.html
    std::vector<std::string_view> split(const std::string_view &sourceLine, const char delimiter);

    // UI
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