#include "catch.hpp"

#include <optional>
#include <variant>

import Ostrich;
using namespace ostrich;
using enum RegisterName;

TEST_CASE("add")
{
    Vm vm{ Source{}, 64 };
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

    SECTION("add from memory")
    {
        using enum AdditiveOperator;
        vm.execute(Mov{ rax, vm.stack().beginning() });
        vm.execute(Mov{ rbx, 5 });
        vm.execute(Push{ rbx });
        vm.execute(Add{ rbx, MemoryAddress{ rax, plus, std::nullopt, 1, plus, 0 } });
        CHECK(vm.cpu().registerValue(rbx) == 10);
    }
}

TEST_CASE("mov")
{
    Vm vm{ Source{}, 64 };
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

    SECTION("mov from memory")
    {
        using enum AdditiveOperator;
        vm.execute(Mov{ rax, vm.stack().beginning() });
        vm.execute(Mov{ rbx, 5 });
        vm.execute(Push{ rbx });
        vm.execute(Mov{ rcx, MemoryAddress{ rax, plus, std::nullopt, 1, plus, 0 } });
        CHECK(vm.cpu().registerValue(rcx) == 5);
    }
}

TEST_CASE("push/pop")
{
    Vm vm{ Source{}, 64 };
    vm.execute(Mov{ rax, 3 });
    vm.execute(Push{ rax });
    CHECK(vm.stack().load(vm.stack().beginning()) == 3);
    vm.execute(Pop{ rbx });
    CHECK(vm.cpu().registerValue(rbx) == 3);
}

TEST_CASE("loadEffectiveAddress")
{
    using enum AdditiveOperator;
    Vm vm{ Source{}, 0 };
    vm.execute(Mov{ rax, 30 });
    vm.execute(Mov{ rbx, 5 });

    // Base only
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, plus, std::nullopt, 1, plus, 0 }) == 30);

    // Base and index
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, plus, rbx, 2, plus, 0 }) == 40);
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, minus, rbx, 1, plus, 0 }) == 25);

    // Base and displacement
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, plus, std::nullopt, 1, plus, 5 }) == 35);
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, plus, std::nullopt, 1, minus, 5 }) == 25);

    // Base and index and displacement
    CHECK(vm.cpu().loadEffectiveAddress(MemoryAddress{ rax, plus, rbx, 2, minus, 2 }) == 38);
}
