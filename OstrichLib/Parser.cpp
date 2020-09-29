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

    template <InstructionSingleRegister InstructionType, OperandList Operands>
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
            return parseInstructionWithSourceAndDestination<Mov>(operands);
        }
        else
        {
            throw std::runtime_error(
            fmt::format("Failed to parse '{}', instruction '{}' not recognized", sourceLine, instruction));
        }
    };

    // TODO maybe implement more things in terms of parseOut, then we can also detect if there's more stuff left etc.

    std::tuple<AdditiveOperator, std::string_view> parseOutAdditiveOperator(const std::string_view &str)
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

    std::tuple<RegisterName, std::string_view> parseOutRegister(const std::string_view &str)
    {
        if(str.size() < 3)
        {
            throw std::runtime_error{ fmt::format("Expected a register name, found '{}'", str) };
        }
        return { parseRegister(str.substr(0, 3)), str.substr(3) };
    }

    std::tuple<uint8_t, std::string_view> parseOutUint8t(const std::string_view &str)
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

    std::string_view::size_type firstOfOrEnd(const std::string_view &string, const std::string_view &search)
    {
        const auto firstOrEnd = string.find_first_of(search);
        if(firstOrEnd == std::string_view::npos)
        {
            return string.size();
        }
        return firstOrEnd;
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

    MemoryAddress parseMemoryAddress(const std::string_view &memoryAddress)
    {
        MemoryAddress memAddress;
        std::string_view input;
        // Base
        std::tie(memAddress.base, input) = parseOutRegister(memoryAddress);
        // Index
        if(!input.empty() && !std::isdigit(peekOrThrow(input, 1)[0]))
        {
            std::tie(memAddress.indexOperator, input) = parseOutAdditiveOperator(input);
            const auto hasScale = input[0] == '(';
            if(hasScale)
            {
                input = input.substr(1);
            }
            std::tie(memAddress.index, input) = parseOutRegister(input);
            if(hasScale)
            {
                input = consumeOrThrow(input, "*");
                std::tie(memAddress.scale, input) = parseOutUint8t(input);
                input = consumeOrThrow(input, ")");
            }
        }
        // Displacement
        if(!input.empty())
        {
            std::tie(memAddress.displacementOperator, input) = parseOutAdditiveOperator(input);
            std::tie(memAddress.displacement, input) = parseOutUint8t(input);
        }
        if(!input.empty())
        {
            throw std::runtime_error{ fmt::format("Unexpected trailing characters: '{}'", input) };
        }
        return memAddress;
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
