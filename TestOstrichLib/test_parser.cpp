#include "catch.hpp"
#include <fmt/core.h>

#include <iostream>
#include <optional>
#include <ostream>
#include <variant>

import Ostrich;

using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using namespace ostrich;
using namespace ostrich::parser;
using enum RegisterName;

namespace
{
    template <InstructionAny InstructionType>
    void checkInstruction(const InstructionType &expected, const Instruction &parsedInstruction)
    {
        INFO(fmt::format("Checking '{}' == '{}'", expected.toString(),
                         std::visit([](const auto i) { return i.toString(); }, parsedInstruction)))
        CHECK(expected == std::get<InstructionType>(parsedInstruction));
    }
} // namespace

TEST_CASE("Instructions with only single register operand")
{
    checkInstruction(Inc{ rax }, parseInstruction("inc rax"));
    checkInstruction(Dec{ rbx }, parseInstruction("dec rbx"));
    checkInstruction(Push{ rax }, parseInstruction("push rax"));
    checkInstruction(Pop{ rbx }, parseInstruction("pop rbx"));

    CHECK_THROWS_WITH(parseInstruction("inc"), Contains("Failed to parse operands from ''"));
    CHECK_THROWS_WITH(parseInstruction("inc rax rbx"),
                      Contains("Trailing output ' rbx' in 'inc rax rbx'"));
}

TEST_CASE("Add")
{
    checkInstruction(Add{ .destination = rax, .source = rbx }, parseInstruction("add rax rbx"));
    checkInstruction(Add{ .destination = rax, .source = 42 }, parseInstruction("add rax 42"));
    checkInstruction(Add{ .destination = rax, .source = 0x123 }, parseInstruction("add rax 0x123"));

    CHECK_THROWS_WITH(parseInstruction("add"), Contains("Failed to parse operands from ''"));
}

TEST_CASE("Mov")
{
    checkInstruction(Mov{ .destination = rbx, .source = 0xff }, parseInstruction("mov rbx 0xff"));
    checkInstruction(Mov{ .destination = rbx, .source = 10 }, parseInstruction("mov rbx 10"));
    checkInstruction(Mov{ .destination = rbx, .source = rax }, parseInstruction("mov rbx rax"));
    using enum AdditiveOperator;
    checkInstruction(Mov{ .destination = rsi, .source = MemoryAddress{ rax, plus, rbx, 2, minus, 4 } },
                     parseInstruction("mov rsi qword ptr [rax+(rbx*2)-4]"));

    CHECK_THROWS_WITH(parseInstruction("mov"), Contains("Failed to parse operands from ''"));
    CHECK_THROWS_WITH(parseInstruction("mov rax rbx rcx"),
                      Contains("Trailing output ' rcx' in 'mov rax rbx rcx'"));
}

MemoryAddress parseAndReturnMemoryAddress(const std::string_view &str)
{
    const auto [memoryAddress, _] = parseMemoryAddress(str);
    return memoryAddress;
}

TEST_CASE("Memory address")
{
    using enum AdditiveOperator;
    CHECK(MemoryAddress{ rax } == parseAndReturnMemoryAddress("rax"));
    CHECK(MemoryAddress{ rbx, plus, std::nullopt, 1, plus, 4 } ==
          parseAndReturnMemoryAddress("rbx+4"));
    CHECK(MemoryAddress{ rbx, plus, std::nullopt, 1, minus, 2 } ==
          parseAndReturnMemoryAddress("rbx-2"));
    CHECK(MemoryAddress{ rbx, plus, rcx, 1, plus, 0 } == parseAndReturnMemoryAddress("rbx+rcx"));
    CHECK(MemoryAddress{ rbx, plus, rdx, 2, plus, 0 } ==
          parseAndReturnMemoryAddress("rbx+(rdx*2)"));
    CHECK(MemoryAddress{ rbx, minus, rax, 1, minus, 8 } ==
          parseAndReturnMemoryAddress("rbx-rax-8"));
    CHECK(MemoryAddress{ rax, minus, rbx, 4, minus, 2 } ==
          parseAndReturnMemoryAddress("rax+(rbx*4)-2"));

    // TODO standardize error messages, use Equals
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress(""), Contains("Expected a register name"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("ra"), Contains("Expected a register name"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("raxrbx"),
                      Equals("Unknown register name 'raxrbx'"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("rax+"), Contains("Failed to parse integer"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("rax+rbx-"), Contains("Failed to parse integer"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("rax+rbx+256"),
                      Equals(
                      "Failed to parse integer from '256': Expected a number <= 255, but got 256"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("rax+(rbx)"), Equals("Expected '*', found ')'"));
    CHECK_THROWS_WITH(parseAndReturnMemoryAddress("rax+(rbx*2"), Equals("Expected ')', found ''"));
}

TEST_CASE("Unknown instructions or empty source lines")
{
    CHECK_THROWS_WITH(parseInstruction("lol wat"), Contains("Failed to parse 'lol wat'"));
    CHECK_THROWS_WITH(parseInstruction("wat"), Contains("Failed to parse 'wat'"));
    CHECK_THROWS_WITH(parseInstruction(""), Contains("Failed to parse empty source line"));
}

TEST_CASE("Parsing full source")
{
    CHECK(parse(std::string_view("")).empty());
    CHECK(Instruction{ Inc{ rax } } == parse(std::string_view("inc rax")).at(0));
    CHECK(Instruction{ Mov{ rbx, 2 } } == parse(std::string_view("mov rbx 2")).at(0));
    CHECK_THAT(parse(std::string_view("inc rax\ndec rbx")),
               Equals(std::vector{ Instruction{ Inc{ rax } }, Instruction{ Dec{ rbx } } }));
}