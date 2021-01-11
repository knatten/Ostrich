module;

#include <fmt/core.h>

#include <array>
#include <filesystem>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>
export module Ostrich;

namespace ostrich
{
    // Registers
    export enum class RegisterName { rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp };
    constexpr size_t registerCount{ 8 };
    export std::string toString(RegisterName registerName);
    export std::ostream &operator<<(std::ostream &os, RegisterName registerName);

    export struct Register
    {
        RegisterName registerName;
        uint64_t value;
    };

    // Memory address
    export enum class AdditiveOperator { plus, minus };
    export std::string toString(AdditiveOperator additiveOperator);
    export std::ostream &operator<<(std::ostream &os, AdditiveOperator additiveOperator);

    export struct MemoryAddress
    {
        RegisterName base;
        AdditiveOperator indexOperator{ AdditiveOperator::plus };
        std::optional<RegisterName> index{ std::nullopt };
        uint8_t scale{ 1 };
        AdditiveOperator displacementOperator{ AdditiveOperator::plus };
        uint64_t displacement{ 0 };

        std::string toString() const;
        std::string toShortString() const;
    };

    export bool operator==(const MemoryAddress &lhs, const MemoryAddress &rhs)
    {
        return lhs.base == rhs.base && lhs.index == rhs.index && lhs.scale == rhs.scale &&
               lhs.displacement == rhs.displacement;
    }

    export std::ostream &operator<<(std::ostream &os, MemoryAddress memoryAddress)
    {
        os << memoryAddress.toString();
        return os;
    }

    // Instructions
    export using RegisterOrImmediateOrMemory = std::variant<RegisterName, uint64_t, MemoryAddress>;

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
        RegisterOrImmediateOrMemory source;
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
        RegisterOrImmediateOrMemory source;
        std::string toString() const;
    };

    export using Instruction = std::variant<Inc, Dec, Add, Push, Pop, Mov>;
    export using Source = std::vector<Instruction>;

    export template <typename InstructionType>
    concept InstructionSingleRegister =
    std::is_same_v<InstructionType, Inc> || std::is_same_v<InstructionType, Dec> ||
    std::is_same_v<InstructionType, Push> || std::is_same_v<InstructionType, Pop>;

    export template <typename InstructionType>
    concept InstructionSourceDestination =
    std::is_same_v<InstructionType, Mov> || std::is_same_v<InstructionType, Add>;

    export template <typename InstructionType>
    concept InstructionAny =
    InstructionSingleRegister<InstructionType> || InstructionSourceDestination<InstructionType>;

    export template <InstructionAny LhsInstruction, InstructionAny RhsInstruction>
    bool operator==(const LhsInstruction &lhs, const RhsInstruction &rhs)
    {
        return false;
    }

    export template <InstructionSingleRegister LhsInstruction, InstructionSingleRegister RhsInstruction>
    bool operator==(const LhsInstruction &lhs, const RhsInstruction &rhs)
    {
        return std::is_same_v<LhsInstruction, RhsInstruction> && lhs.registerName == rhs.registerName;
    }

    export template <InstructionSourceDestination LhsInstruction, InstructionSourceDestination RhsInstruction>
    bool operator==(const LhsInstruction &lhs, const RhsInstruction &rhs)
    {
        return std::is_same_v<LhsInstruction, RhsInstruction> && lhs.source == rhs.source &&
               lhs.destination == rhs.destination;
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
        Stack(uint64_t size, uint64_t beginning);

        const std::vector<uint8_t> content() const;
        uint64_t beginning() const;
        void store(uint64_t address, uint64_t value);
        uint64_t load(uint64_t address) const;

    private:
        uint64_t m_size;
        uint64_t m_beginning;
        std::vector<uint8_t> m_content;
    };

    // Cpu
    export class Cpu
    {
    public:
        Cpu(Stack &stack, Source &source);
        Cpu(Stack &stack, Source &source, size_t nextInstruction, std::array<Register, registerCount> registers);

        void step();
        void execute(const Instruction &instruction);
        size_t nextInstruction() const;
        const std::array<Register, registerCount> registers() const;

        const uint64_t &registerValue(RegisterName r) const;
        uint64_t memoryValue(const MemoryAddress &address) const;
        uint64_t loadEffectiveAddress(const MemoryAddress &address) const;

    private:
        uint64_t &registerValue(RegisterName r);
        uint64_t readValue(RegisterOrImmediateOrMemory r);

        Stack *m_stack;
        Source *m_source;
        size_t m_nextInstruction{ 0 };
        std::array<Register, registerCount> m_registers;
    };

    // Vm
    export class Vm
    {
    public:
        Vm(Source source, size_t stackSize);

        void load(Source source);
        void step();
        void execute(const Instruction &instruction);
        void restorePreviousState();
        const Cpu &cpu() const;
        const Stack &stack() const;
        const Source &source() const;

        // TODO make this private and members private and declare swap a friend. Blocked by msvc issue:
        // https://developercommunity.visualstudio.com/content/problem/1191747/friends-declared-in-nested-classes-dont-work-with.html
        class State
        {
        public:
            State(Source source, size_t stackSize);
            State(const State &other);
            State &operator=(State other) noexcept;

            Stack m_stack;
            Source m_source;
            Cpu m_cpu{ m_stack, m_source };
        };

    private:
        State &state();
        const State &state() const;
        void saveState();

        static constexpr uint64_t stackTop{ 0xffff };
        std::vector<State> m_states;
    };

    void swap(Vm::State &lhs, Vm::State &rhs) noexcept;

    // Parser
    export Instruction parseInstruction(const std::string_view &sourceLine);
    export std::tuple<MemoryAddress, std::string_view> parseMemoryAddress(const std::string_view &memoryAddress);
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

    // Tokens
    export namespace tokenizer
    {
        export struct Word
        {
            static constexpr const char *tokenName = "Word";
            std::string value;
        };

        export struct Number
        {
            static constexpr const char *tokenName = "Number";
            std::string value;
        };

        export struct Comma
        {
            static constexpr const char *tokenName = "Comma";
            std::string value{ "," };
        };

        export struct ArithmeticOperator
        {
            static constexpr const char *tokenName = "ArithmeticOperator";
            std::string value;
        };

        export struct LeftBracket
        {
            static constexpr const char *tokenName = "LeftBracket";
            std::string value{ "[" };
        };

        export struct RightBracket
        {
            static constexpr const char *tokenName = "RightBracket";
            std::string value{ "]" };
        };

        export struct LeftParenthesis
        {
            static constexpr const char *tokenName = "LeftParenthesis";
            std::string value{ "(" };
        };

        export struct RightParenthesis
        {
            static constexpr const char *tokenName = "RightParenthesis";
            std::string value{ ")" };
        };

        export using Token =
        std::variant<Word, Number, Comma, ArithmeticOperator, LeftBracket, RightBracket, LeftParenthesis, RightParenthesis>;

        export template <typename T>
        concept TokenAny = std::is_same_v<T, Word> || std::is_same_v<T, Number> ||
                           std::is_same_v<T, Comma> || std::is_same_v<T, ArithmeticOperator> ||
                           std::is_same_v<T, LeftBracket> || std::is_same_v<T, RightBracket> ||
                           std::is_same_v<T, LeftParenthesis> || std::is_same_v<T, RightParenthesis>;

        export template <TokenAny T>
        std::ostream &operator<<(std::ostream &os, const T &w)
        {
            os << fmt::format("{}{{{}}}", T::tokenName, w.value);
            return os;
        }

        export template <TokenAny LhsToken, TokenAny RhsToken>
        bool operator==(const LhsToken &lhs, const RhsToken &rhs)
        {
            return std::is_same_v<LhsToken, RhsToken> && lhs.value == rhs.value;
        }

        export std::vector<Token> tokenize(std::string_view input);
    } // namespace tokenizer
} // namespace ostrich
