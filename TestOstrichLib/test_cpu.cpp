#include "catch.hpp"

#include <variant>

import Ostrich;
using namespace ostrich;
using enum RegisterName;

TEST_CASE("add")
{
    Vm vm{ Source{}, 0 };
    SECTION("add two registers")
    {
        vm.execute(Mov{ rax, 2 });
        vm.execute(Mov{ rbx, 4 });
        vm.execute(Add{ rax, rbx });
        auto &cpu = vm.cpu();
        CHECK(cpu.registerValue(rax) == 6);
        CHECK(cpu.registerValue(rbx) == 4);
    }

    SECTION("add immediate value")
    {
        vm.execute(Mov{ rax, 3 });
        vm.execute(Add{ rax, 6 });
        auto &cpu = vm.cpu();
        CHECK(cpu.registerValue(rax) == 9);
    }
}
TEST_CASE("mov")
{
    Vm vm{ Source{}, 0 };
    SECTION("mov two registers")
    {
        vm.execute(Mov{ rax, 2 });
        vm.execute(Mov{ rbx, 4 });
        vm.execute(Mov{ rax, rbx });
        auto &cpu = vm.cpu();
        CHECK(cpu.registerValue(rax) == 4);
        CHECK(cpu.registerValue(rbx) == 4);
    }

    SECTION("mov immediate value")
    {
        vm.execute(Mov{ rax, 3 });
        CHECK(vm.cpu().registerValue(rax) == 3);
    }
}