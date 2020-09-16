#include "catch.hpp"

#include <variant>

import Ostrich;

using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using namespace ostrich; //TODO remove qualifiers in the source code now that we have this here
using enum RegisterName;

namespace
{
    // TODO test concepts here?
    // TODO now we can use equality operator instead
    template <typename InstructionType>
    InstructionType checkInstruction(const ostrich::Instruction &parsedInstruction)
    {
        INFO("Checking " << std::visit([](const auto i) { return i.toString(); }, parsedInstruction))
        REQUIRE(std::holds_alternative<InstructionType>(parsedInstruction));
        return std::get<InstructionType>(parsedInstruction);
    }

} // namespace

TEST_CASE("Inc")
{
    const auto inc = checkInstruction<ostrich::Inc>(ostrich::parseInstruction("inc rax"));
    REQUIRE(inc.registerName == ostrich::RegisterName::rax);
}

TEST_CASE("Dec")
{
    const auto dec = checkInstruction<ostrich::Dec>(ostrich::parseInstruction("dec rbx"));
    REQUIRE(dec.registerName == ostrich::RegisterName::rbx);
}

TEST_CASE("Push")
{
    const auto push = checkInstruction<ostrich::Push>(ostrich::parseInstruction("push rax"));
    REQUIRE(push.registerName == ostrich::RegisterName::rax);
}

TEST_CASE("Pop")
{
    const auto pop = checkInstruction<ostrich::Pop>(ostrich::parseInstruction("pop rbx"));
    REQUIRE(pop.registerName == ostrich::RegisterName::rbx);
}

TEST_CASE("Add")
{
    const auto add = checkInstruction<ostrich::Add>(ostrich::parseInstruction("add rax rbx"));
    REQUIRE(add.destination == ostrich::RegisterName::rax);
    REQUIRE(add.source == ostrich::RegisterName::rbx);
}

TEST_CASE("Mov")
{
    const auto mov = checkInstruction<ostrich::Mov>(ostrich::parseInstruction("mov rbx ff"));
    REQUIRE(mov.destination == ostrich::RegisterName::rbx);
    REQUIRE(mov.value == 0xff);
}

TEST_CASE("Unknown instructions or empty source lines")
{
    CHECK_THROWS_WITH(ostrich::parseInstruction("lol wat"), Contains("Failed to parse 'lol wat'"));
    CHECK_THROWS_WITH(ostrich::parseInstruction("wat"), Contains("Failed to parse 'wat'"));
    CHECK_THROWS_WITH(ostrich::parseInstruction(""),
                      Contains("Failed to parse empty source line!"));
}

TEST_CASE("Parsing full source")
{
    CHECK(parse("").empty());
    CHECK(Instruction{ Inc{ rax } } == parse("inc rax").at(0));
    CHECK(Instruction{ Mov{ rbx, 2 } } == parse("mov rbx 2").at(0));
    CHECK_THAT(parse("inc rax\ndec rbx"), Equals(std::vector{ Instruction{ Inc{ rax } }, Instruction{ Dec{ rbx } } }));
}