module;

#include <fmt/core.h>

#include <algorithm>
#include <fstream>
#include <ranges>
#include <stdexcept>
#include <string_view>
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
        static_assert(registerCount == 5, "Don't forget to update this!");
        throw std::runtime_error(fmt::format("Unknown register name '{}'", reg));
    }

    uint64_t parseImmediateValue(const std::string_view &value)
    {
        return value.starts_with("0x") ? std::stoull(std::string(value), 0, 16) :
                                         std::stoull(std::string(value));
    }

    // TODO constrain that Operands is ForwardIterator of string_view (or "something stringish" if that concept exist)
    template <InstructionSingleRegisterOperand InstructionType, typename Operands>
    InstructionType parseInstructionWithSingleRegister(const Operands &operands)
    {
        return InstructionType{ parseRegister(operands[0]) };
    }

    // TODO constrain InstructionType
    template <typename InstructionType, typename Operands>
    InstructionType parseInstructionWithSourceAndDestination(const Operands &operands)
    {
        return InstructionType{ parseRegister(operands[0]), parseRegister(operands[1]) };
    }

    // TODO delete and use parseInstructionWithSourceAndDestination instead
    template <typename Operands>
    Mov parseMov(const Operands &operands)
    {
        // TODO support leading "0x", then parse as hex, otherwise parse as base 10
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
