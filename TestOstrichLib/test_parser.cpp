#include "catch.hpp"
#include <fmt/core.h>

#include <iostream>
#include <ostream>
#include <variant>

import Ostrich;

using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using namespace ostrich;
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

    CHECK_THROWS_WITH(parseInstruction("inc"),
                      Contains("Wrong number of operands, got 0, expected 1"));
    CHECK_THROWS_WITH(parseInstruction("inc rax rbx"),
                      Contains("Wrong number of operands, got 2, expected 1"));
}

TEST_CASE("Add")
{
    checkInstruction(Add{ .destination = rax, .source = rbx }, parseInstruction("add rax rbx"));
    checkInstruction(Add{ .destination = rax, .source = 42 }, parseInstruction("add rax 42"));
    checkInstruction(Add{ .destination = rax, .source = 0x123 }, parseInstruction("add rax 0x123"));

    CHECK_THROWS_WITH(parseInstruction("add"),
                      Contains("Wrong number of operands, got 0, expected 2"));
    CHECK_THROWS_WITH(parseInstruction("add rax rbx rcx"),
                      Contains("Wrong number of operands, got 3, expected 2"));
}

TEST_CASE("Mov")
{
    checkInstruction(Mov{ .destination = rbx, .value = 0xff }, parseInstruction("mov rbx 0xff"));
    checkInstruction(Mov{ .destination = rbx, .value = 10 }, parseInstruction("mov rbx 10"));

    CHECK_THROWS_WITH(parseInstruction("mov"),
                      Contains("Wrong number of operands, got 0, expected 2"));
    CHECK_THROWS_WITH(parseInstruction("mov rax rbx rcx"),
                      Contains("Wrong number of operands, got 3, expected 2"));
}

TEST_CASE("Unknown instructions or empty source lines")
{
    CHECK_THROWS_WITH(parseInstruction("lol wat"), Contains("Failed to parse 'lol wat'"));
    CHECK_THROWS_WITH(parseInstruction("wat"), Contains("Failed to parse 'wat'"));
    CHECK_THROWS_WITH(parseInstruction(""), Contains("Failed to parse empty source line!"));
}

TEST_CASE("Parsing full source")
{
    CHECK(parse(std::string_view("")).empty());
    CHECK(Instruction{ Inc{ rax } } == parse(std::string_view("inc rax")).at(0));
    CHECK(Instruction{ Mov{ rbx, 2 } } == parse(std::string_view("mov rbx 2")).at(0));
    CHECK_THAT(parse(std::string_view("inc rax\ndec rbx")),
               Equals(std::vector{ Instruction{ Inc{ rax } }, Instruction{ Dec{ rbx } } }));
}