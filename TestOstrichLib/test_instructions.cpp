#include "catch.hpp"
#include <iostream>
#include <variant>

import Ostrich;

using namespace ostrich;
using enum RegisterName;
TEST_CASE("Equality")
{
    CHECK(Inc{ rax } == Inc{ rax });
    CHECK(Inc{ rax } != Inc{ rbx });

    CHECK(Add{ rax, rax } == Add{ rax, rax });
    CHECK(Add{ rax, rax } != Add{ rax, rbx });
}

TEST_CASE("Different types compare unequal")
{
    CHECK(Inc{ rax } != Dec{ rax });
    CHECK(Inc{ rax } != Mov{ rax, 1 });
}

TEST_CASE("Can compare using the variant")
{
    CHECK(Instruction{ Inc{ rax } } == Instruction{ Inc{ rax } });
    CHECK(Instruction{ Inc{ rax } } != Instruction{ Inc{ rbx } });

    CHECK(Instruction{ Inc{ rax } } != Instruction{ Dec{ rbx } });
    CHECK(Instruction{ Add{ rax, rbx } } != Instruction{ Mov{ rbx , 2} });
}