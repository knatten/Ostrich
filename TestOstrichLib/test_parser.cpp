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

TEST_CASE("Inc")
{
    checkInstruction(Inc{ rax }, parseInstruction("inc rax"));
}

TEST_CASE("Dec")
{
    checkInstruction(Dec{ rbx }, parseInstruction("dec rbx"));
}

TEST_CASE("Push")
{
    checkInstruction(Push{ rax }, parseInstruction("push rax"));
}

TEST_CASE("Pop")
{
    checkInstruction(Pop{ rbx }, parseInstruction("pop rbx"));
}

TEST_CASE("Add")
{
    checkInstruction(Add{ .destination = rax, .source = rbx }, parseInstruction("add rax rbx"));
}

TEST_CASE("Mov")
{
    checkInstruction(Mov{ .destination = rbx, .value = 0xff }, parseInstruction("mov rbx 0xff"));
    checkInstruction(Mov{ .destination = rbx, .value = 10 }, parseInstruction("mov rbx 10"));
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