#include "catch.hpp"
#include <iostream>
#include <optional>
#include <sstream>
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
    CHECK(Instruction{ Add{ rax, rbx } } != Instruction{ Mov{ rbx, 2 } });
}

TEST_CASE("Can ostream instructions")
{
    std::stringstream ss1;
    ss1 << Push{ rbx };
    CHECK("push rbx" == ss1.str());

    std::stringstream ss2;
    ss2 << Instruction{ Push{ rax } };
    CHECK("push rax" == ss2.str());
}

TEST_CASE("Can ostream RegisterName")
{
    std::stringstream ss1;
    ss1 << RegisterName::rdx;
    CHECK("rdx" == ss1.str());
}

TEST_CASE("toString")
{
    CHECK("inc  rax" == Inc{ rax }.toString());
    CHECK("dec  rbx" == Dec{ rbx }.toString());
    CHECK("add  rcx 0x123" == Add{ rcx, 0x123 }.toString());
    CHECK("add  rcx 0x1234567" == Add{ rcx, 0x1234567 }.toString());
    CHECK("add  rcx rax" == Add{ rcx, rax }.toString());
    CHECK("push rax" == Push{ rax }.toString());
    CHECK("pop  rbx" == Pop{ rbx }.toString());
    CHECK("mov  rax rbx" == Mov{ rax, rbx }.toString());
    CHECK("mov  rax 0x1" == Mov{ rax, 1 }.toString());
    using enum AdditiveOperator;
    CHECK("mov  rsi qword ptr [rax+(rbx*2)-4]" ==
          Mov{ rsi, MemoryAddress{ rax, plus, rbx, 2, minus, 4 } }.toString());
}

TEST_CASE("Memory address equality")
{
    using enum AdditiveOperator;
    CHECK(MemoryAddress{ rax } == MemoryAddress{ rax });
    CHECK(MemoryAddress{ rax } != MemoryAddress{ rbx });

    CHECK(MemoryAddress{ rax, plus } != MemoryAddress{ rbx, minus });

    CHECK(MemoryAddress{ rax, plus, std::nullopt } != MemoryAddress{ rbx, plus, rax });
    CHECK(MemoryAddress{ rax, plus, rbx } != MemoryAddress{ rbx, plus, rax });

    CHECK(MemoryAddress{ rax, plus, rax, 0 } != MemoryAddress{ rax, plus, rax, 1 });

    CHECK(MemoryAddress{ rax, plus, rax, 0, minus } != MemoryAddress{ rax, plus, rax, 1, minus });

    CHECK(MemoryAddress{ rax, plus, rax, 0, minus, 0 } != MemoryAddress{ rax, plus, rax, 0, minus, 1 });

    CHECK(MemoryAddress{ rbx, plus, rax, 1, minus, 1 } == MemoryAddress{ rbx, plus, rax, 1, minus, 1 });
}
