module;

#include <fmt/core.h>

#include <algorithm>
#include <fstream>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <variant>
module Ostrich;


namespace ostrich
{
    std::vector<std::string_view> split(const std::string_view &sourceLine, const char delimiter)
    {
        std::vector<std::string_view> output;
        auto first = sourceLine.begin();

        while(first != sourceLine.end())
        {
            const auto second = std::find(first, std::cend(sourceLine), delimiter);
            if(first != second)
            {
                output.emplace_back(sourceLine.substr(std::distance(sourceLine.begin(), first),
                                                      std::distance(first, second)));
            }

            if(second == sourceLine.end())
                break;

            first = std::next(second);
        }

        return output;
    }

    RegisterName parseRegister(const std::string_view &reg)
    {
        using enum RegisterName;
        if(reg == "rax")
        {
            return rax;
        }
        if(reg == "rbx")
        {
            return rbx;
        }
        if(reg == "rcx")
        {
            return rcx;
        }
        if(reg == "rdx")
        {
            return rdx;
        }
        if(reg == "rsi")
        {
            return rsi;
        }
        if(reg == "rdi")
        {
            return rdi;
        }
        if(reg == "rbp")
        {
            return rbp;
        }
        if(reg == "rsp")
        {
            return rsp;
        }
        static_assert(registerCount == 8, "Don't forget to update this!");
        throw std::runtime_error(fmt::format("Unknown register name '{}'", reg));
    }

    uint64_t parseImmediateValue(const std::string_view &value)
    {
        return value.starts_with("0x") ? std::stoull(std::string(value), 0, 16) :
                                         std::stoull(std::string(value));
    }

    RegisterNameOrImmediate parseRegisterNameOrImmediate(const std::string_view &value)
    {
        return std::isdigit(value[0]) ? RegisterNameOrImmediate{ parseImmediateValue(value) } :
                                        RegisterNameOrImmediate{ parseRegister(value) };
    }

    template <typename T>
    concept OperandList =
    std::forward_iterator<std::ranges::iterator_t<T>> &&std::is_same_v<std::ranges::range_value_t<T>, std::string_view>;

    template <InstructionSingleRegisterOperand InstructionType, OperandList Operands>
    InstructionType parseInstructionWithSingleRegister(const Operands &operands)
    {
        if(operands.size() != 1)
        {
            throw std::runtime_error(
            fmt::format("Wrong number of operands, got {}, expected {}.", operands.size(), 1));
        }
        return InstructionType{ parseRegister(operands[0]) };
    }

    template <typename InstructionType, OperandList Operands>
    requires(std::is_same_v<InstructionType, Mov> || std::is_same_v<InstructionType, Add>) InstructionType
    parseInstructionWithSourceAndDestination(const Operands &operands)
    {
        if(operands.size() != 2)
        {
            throw std::runtime_error(
            fmt::format("Wrong number of operands, got {}, expected {}.", operands.size(), 2));
        }
        return InstructionType{ parseRegister(operands[0]), parseRegisterNameOrImmediate(operands[1]) };
    }

     //TODO delete and use parseInstructionWithSourceAndDestination instead
    template <typename Operands>
    Mov parseMov(const Operands &operands)
    {
        if(operands.size() != 2)
        {
            throw std::runtime_error(
            fmt::format("Wrong number of operands, got {}, expected {}.", operands.size(), 2));
        }
        return Mov{ parseRegister(operands[0]), parseImmediateValue(operands[1]) };
    }

    Instruction parseInstruction(const std::string_view &sourceLine)
    {
        const auto tokens = split(sourceLine, ' ');
        if(tokens.empty())
        {
            throw std::runtime_error("Failed to parse empty source line!");
        }
        const auto instruction = tokens[0];
        const auto operands = tokens | std::views::drop(1);
        if(instruction == "inc")
        {
            return parseInstructionWithSingleRegister<Inc>(operands);
        }
        if(instruction == "dec")
        {
            return parseInstructionWithSingleRegister<Dec>(operands);
        }
        if(instruction == "push")
        {
            return parseInstructionWithSingleRegister<Push>(operands);
        }
        if(instruction == "pop")
        {
            return parseInstructionWithSingleRegister<Pop>(operands);
        }
        if(instruction == "add")
        {
            return parseInstructionWithSourceAndDestination<Add>(operands);
        }
        if(instruction == "mov")
        {
            return parseMov(operands);
        }
        else
        {
            throw std::runtime_error(
            fmt::format("Failed to parse '{}', instruction '{}' not recognized", sourceLine, instruction));
        }
    };

    Source parse(const std::string_view &sourceText)
    {
        Source source;
        const auto lines = split(sourceText, '\n');
        for(const auto &line : lines)
        {
            source.push_back(parseInstruction(line));
        }
        return source;
    }

    Source parse(const std::filesystem::path &sourcePath)
    {
        Source source;
        std::ifstream file(sourcePath.string());
        auto f = sourcePath.filename();
        auto p = sourcePath.parent_path();
        if(!file.is_open())
        {
            throw std::runtime_error(fmt::format("Failed to open '{}'", sourcePath.string()));
        }
        std::string line;
        while(std::getline(file, line))
        {
            source.push_back(parseInstruction(line));
        }
        return source;
    }
} // namespace ostrich
