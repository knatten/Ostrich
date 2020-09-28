#include "catch.hpp"

#include <variant>

import Ostrich;
using namespace ostrich;
using enum RegisterName;

TEST_CASE("add two registers")
{
    Vm vm{ Source{}, 0 };
    vm.execute(Mov{ rax, 2 });
    vm.execute(Mov{ rbx, 4 });
    vm.execute(Add{ rax, rbx });
    auto &cpu = vm.cpu();
    CHECK(cpu.registerValue(rax) == 6);
    CHECK(cpu.registerValue(rbx) == 4);
}

TEST_CASE("add immediate value")
{
    Vm vm{ Source{}, 0 };
    vm.execute(Mov{ rax, 3 });
    vm.execute(Add{ rax, 6 });
    auto &cpu = vm.cpu();
    CHECK(cpu.registerValue(rax) == 9);
}
