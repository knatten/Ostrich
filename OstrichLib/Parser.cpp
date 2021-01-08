module;

#include <fmt/core.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <tuple>
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

    std::string_view skipSpace(const std::string_view &str)
    {
        if(str.empty())
        {
            return str;
        }
        const auto firstNonSpace = str.find_first_not_of(' ');
        return firstNonSpace == std::string_view::npos ? str.substr(str.size()) : str.substr(firstNonSpace);
    }

    std::string_view peekOrThrow(const std::string_view &str, size_t pos, size_t count = std::string_view::npos)
    {
        const auto minLength = pos + (count == std::string_view::npos ? 0 : count);
        if(minLength >= str.size())
        {
            throw std::runtime_error{
                fmt::format("Unexpected end of input, tried to parse {} characters from '{}'", minLength, str)
            };
        }
        return str.substr(pos, count);
    }

    std::string_view consumeOrThrow(const std::string_view &str, const std::string_view &expected)
    {
        if(!str.starts_with(expected))
        {
            throw std::runtime_error{ fmt::format("Expected '{}', found '{}'", expected, str) };
        }
        return str.substr(expected.size());
    }

    std::tuple<uint8_t, std::string_view> parseUint8t(const std::string_view &str)
    {
        std::size_t pos;
        try
        {
            int i = std::stoi(std::string{ str }, &pos);
            if(i > std::numeric_limits<uint8_t>::max())
            {
                throw std::runtime_error{ fmt::format("Expected a number <= {}, but got {}",
                                                      std::numeric_limits<uint8_t>::max(), i) };
            }
            return { static_cast<uint8_t>(i), str.substr(pos) };
        }
        catch(const std::exception &e)
        {
            throw std::runtime_error{ fmt::format("Failed to parse integer from '{}': {}", str, e.what()) };
        }
    }

    bool isNotWordCharacter(char c)
    {
        return !std::isalnum(c) && c != '_';
    }

    std::tuple<std::string_view, std::string_view> parseWord(const std::string_view &str)
    {
        if(str.empty())
        {
            throw std::runtime_error(fmt::format("Failed to parse word from '{}'", str));
        }
        const auto wordEnd =
        std::distance(str.cbegin(), std::find_if(str.cbegin(), str.cend(), isNotWordCharacter));
        return { str.substr(0, wordEnd), str.substr(wordEnd) };
    }

    RegisterName stringToRegisterName(const std::string_view &reg)
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

    std::tuple<uint64_t, std::string_view> parseImmediateValue(const std::string_view &str)
    {
        const auto [value, rest] = parseWord(str);
        const auto result = value.starts_with("0x") ? std::stoull(std::string(value), 0, 16) :
                                                      std::stoull(std::string(value));
        return { result, rest };
    }

    std::tuple<RegisterName, std::string_view> parseRegister(const std::string_view &str)
    {
        if(str.size() < 3)
        {
            throw std::runtime_error{ fmt::format("Expected a register name, found '{}'", str) };
        }
        const auto [reg, rest] = parseWord(str);
        return { stringToRegisterName(reg), rest };
    }

    std::tuple<AdditiveOperator, std::string_view> parseAdditiveOperator(const std::string_view &str)
    {
        if(str[0] == '+')
        {
            return { AdditiveOperator::plus, str.substr(1) };
        }
        if(str[0] == '-')
        {
            return { AdditiveOperator::minus, str.substr(1) };
        }
        throw std::runtime_error{ fmt::format("Failed to parse additive operator, found '{}'", str) };
    }

    std::tuple<MemoryAddress, std::string_view> parseMemoryAddress(const std::string_view &memoryAddress)
    {
        MemoryAddress memAddress;
        std::string_view input;
        // Base
        std::tie(memAddress.base, input) = parseRegister(memoryAddress);
        // Index
        // If we have +/- and then either a register or a parenthesized expression
        if(input.size() >= 2 && (input[0] == '+' || input[0] == '-') &&
           (input[1] == '(' || std::isalpha(input[1])))
        {
            std::tie(memAddress.indexOperator, input) = parseAdditiveOperator(input);
            const auto hasScale = input[0] == '(';
            if(hasScale)
            {
                input = input.substr(1);
            }
            std::tie(memAddress.index, input) = parseRegister(input);
            if(hasScale)
            {
                input = consumeOrThrow(input, "*");
                std::tie(memAddress.scale, input) = parseUint8t(input);
                input = consumeOrThrow(input, ")");
            }
        }
        // Displacement
        if(!input.empty() && (input[0] == '+' || input[0] == '-'))
        {
            std::tie(memAddress.displacementOperator, input) = parseAdditiveOperator(input);
            std::tie(memAddress.displacement, input) = parseUint8t(input);
        }
        return { memAddress, input };
    }

    std::tuple<RegisterOrImmediateOrMemory, std::string_view>
    parseRegisterOrImmediateOrMemory(const std::string_view &str)
    {
        if(str.empty())
        {
            throw std::runtime_error(
            "Exepected a register name, an immediate or a memory address, got empty string");
        }
        if(std::isdigit(str[0]))
        {
            const auto [immediate, rest] = parseImmediateValue(str);
            return { RegisterOrImmediateOrMemory{ immediate }, rest };
        }
        else if(str.starts_with("qword"))
        {
            const auto memAddressBegin = consumeOrThrow(str, "qword ptr [");
            const auto [memAddress, rest] = parseMemoryAddress(memAddressBegin);
            return { memAddress, consumeOrThrow("]", rest) };
        }
        else
        {
            const auto [reg, rest] = parseRegister(str);
            return { RegisterOrImmediateOrMemory{ reg }, rest };
        }
    }

    template <InstructionSingleRegister InstructionType>
    std::tuple<InstructionType, std::string_view>
    parseInstructionWithSingleRegister(const std::string_view &operands)
    {
        try
        {
            const auto [reg, rest] = parseRegister(operands);
            return { InstructionType{ reg }, rest };
        }
        catch(const std::runtime_error &)
        {
            throw std::runtime_error{ fmt::format("Failed to parse operands from '{}'", operands) };
        }
    }


    template <InstructionSourceDestination InstructionType>
    std::tuple<InstructionType, std::string_view>
    parseInstructionWithSourceAndDestination(const std::string_view &operands)
    {
        try
        {
            const auto [destination, sourceStr] = parseRegister(operands);
            const auto [source, rest] = parseRegisterOrImmediateOrMemory(skipSpace(sourceStr));
            return { InstructionType{ destination, source }, rest };
        }
        catch(const std::runtime_error &)
        {
            throw std::runtime_error{ fmt::format("Failed to parse operands from '{}'", operands) };
        }
    }

    std::tuple<Instruction, std::string_view> parseInstructionInternal(const std::string_view &sourceLine)
    {
        if(skipSpace(sourceLine).empty())
        {
            throw std::runtime_error("Failed to parse empty source line");
        }
        // TODO be more forgiving about leading spaces, multiple spaces between operators/operands etc, and test this
        const auto [instruction, rest] = parseWord(sourceLine);
        const auto operands = skipSpace(rest);
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
            return parseInstructionWithSourceAndDestination<Mov>(operands);
        }
        else
        {
            throw std::runtime_error(
            fmt::format("Failed to parse '{}', instruction '{}' not recognized", sourceLine, instruction));
        }
    };

    Instruction parseInstruction(const std::string_view &sourceLine)
    {
        const auto [instruction, rest] = parseInstructionInternal(sourceLine);
        if(!rest.empty())
        {
            throw std::runtime_error{ fmt::format("Trailing output '{}' in '{}'", rest, sourceLine) };
        }
        return instruction;
    }

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
