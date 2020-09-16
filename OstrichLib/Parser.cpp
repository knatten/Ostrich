module;

#include <fmt/core.h>

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <string_view>
module Ostrich;


namespace ostrich
{
    // split_view is not implemented yet, so I stole https://www.bfilipek.com/2018/07/string-view-perf-followup.html
    std::vector<std::string_view> split(const std::string_view &sourceLine)
    {
        std::vector<std::string_view> output;
        auto first = sourceLine.begin();

        while(first != sourceLine.end())
        {
            const auto second = std::find(first, std::cend(sourceLine), ' ');
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
        throw std::runtime_error(fmt::format("Unknown register name '{}'", reg));
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
        return Mov{ parseRegister(operands[0]), std::stoull(std::string(operands[1]), 0, 16) };
    }

    Instruction parseInstruction(const std::string_view &sourceLine)
    {
        const auto tokens = split(sourceLine);
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
} // namespace ostrich
