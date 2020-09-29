#include "catch.hpp"

#include <optional>
#include <sstream>
#include <variant>

import Ostrich;

using namespace ostrich;
using Catch::Matchers::Equals;

TEST_CASE("Additive operator toString")
{
    using enum AdditiveOperator;
    CHECK("+" == toString(plus));
    CHECK("-" == toString(minus));
    CHECK_THROWS_WITH(toString(static_cast<AdditiveOperator>(2)),
                      Equals("No such additive operator"));
}

TEST_CASE("MemoryAddress toString")
{
    using enum RegisterName;
    using enum AdditiveOperator;
    CHECK("rax+(nullopt*1)+0" == MemoryAddress{ rax }.toString());
    CHECK("rax-(rbx*2)-4" == MemoryAddress{ rax, minus, rbx, 2, minus, 4 }.toString());
}

TEST_CASE("MemoryAddress toShortString")
{
    using enum RegisterName;
    using enum AdditiveOperator;
    CHECK("rax" == MemoryAddress{ rax }.toShortString());
    CHECK("rax" == MemoryAddress{ rax, minus, std::nullopt, 4, minus, 0 }.toShortString());
    CHECK("rax-4" == MemoryAddress{ rax, minus, std::nullopt, 4, minus, 4 }.toShortString());
    CHECK("rax+(rbx*2)" == MemoryAddress{ rax, plus, rbx, 2, minus, 0 }.toShortString());
    CHECK("rax+(rbx*2)-8" == MemoryAddress{ rax, plus, rbx, 2, minus, 8 }.toShortString());
}

TEST_CASE("Can ostream AdditiveOpertor")
{
    std::stringstream ss1;
    ss1 << AdditiveOperator::plus;
    CHECK("+" == ss1.str());
}

TEST_CASE("Can ostream MemoryAddress")
{
    using enum RegisterName;
    using enum AdditiveOperator;
    std::stringstream ss1;
    ss1 << MemoryAddress{ rax, plus, rbx, 2, minus, 8 };
    CHECK("rax+(rbx*2)-8" == ss1.str());
}
