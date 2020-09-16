#include "catch.hpp"

#include <variant>

import Ostrich;

using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using namespace ostrich;
using enum RegisterName;

namespace
{
    // TODO test concepts here?
    // TODO now we can use equality operator instead
    template <typename InstructionType>
    InstructionType checkInstruction(const Instruction &parsedInstruction)
    {
        INFO("Checking " << std::visit([](const auto i) { return i.toString(); }, parsedInstruction))
        REQUIRE(std::holds_alternative<InstructionType>(parsedInstruction));
        return std::get<InstructionType>(parsedInstruction);
    }

} // namespace

TEST_CASE("Inc")
{
    const auto inc = checkInstruction<Inc>(parseInstruction("inc rax"));
    REQUIRE(inc.registerName == rax);
}

TEST_CASE("Dec")
{
    const auto dec = checkInstruction<Dec>(parseInstruction("dec rbx"));
    REQUIRE(dec.registerName == rbx);
}

TEST_CASE("Push")
{
    const auto push = checkInstruction<Push>(parseInstruction("push rax"));
    REQUIRE(push.registerName == rax);
}

TEST_CASE("Pop")
{
    const auto pop = checkInstruction<Pop>(parseInstruction("pop rbx"));
    REQUIRE(pop.registerName == rbx);
}

TEST_CASE("Add")
{
    const auto add = checkInstruction<Add>(parseInstruction("add rax rbx"));
    REQUIRE(add.destination == rax);
    REQUIRE(add.source == rbx);
}

TEST_CASE("Mov")
{
    const auto mov = checkInstruction<Mov>(parseInstruction("mov rbx ff"));
    REQUIRE(mov.destination == rbx);
    REQUIRE(mov.value == 0xff);
}

TEST_CASE("Unknown instructions or empty source lines")
{
    CHECK_THROWS_WITH(parseInstruction("lol wat"), Contains("Failed to parse 'lol wat'"));
    CHECK_THROWS_WITH(parseInstruction("wat"), Contains("Failed to parse 'wat'"));
    CHECK_THROWS_WITH(parseInstruction(""), Contains("Failed to parse empty source line!"));
}

TEST_CASE("Parsing full source")
{
    CHECK(parse("").empty());
    CHECK(Instruction{ Inc{ rax } } == parse("inc rax").at(0));
    CHECK(Instruction{ Mov{ rbx, 2 } } == parse("mov rbx 2").at(0));
    CHECK_THAT(parse("inc rax\ndec rbx"),
               Equals(std::vector{ Instruction{ Inc{ rax } }, Instruction{ Dec{ rbx } } }));
}