#include "catch.hpp"

#include <variant>

import Ostrich;

using Catch::Matchers::Contains;

namespace
{
    // TODO test concepts here?
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
    CHECK_THROWS_WITH(ostrich::parseInstruction(""), Contains("Failed to parse empty source line!"));
}