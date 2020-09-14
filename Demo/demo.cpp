#include <iostream>
#include <string_view>

import Ostrich;

void print(const std::string_view &s)
{
    std::cout << "\n" << s << std::endl;
    std::cin.get();
}

int main()
{
    ostrich::Vm vm;
    ostrich::UI ui(120, 28, vm);
    ui.render();
    print("inc rax");

    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rax });
    ui.render();
    print("inc rbx");

    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rbx });
    ui.render();
    print("add rax rbx");

    vm.cpu().execute(
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx });
    ui.render();
    print("push rbx");

    vm.cpu().execute(ostrich::Push{ ostrich::RegisterName::rbx });
    ui.render();
    print("push rax");

    vm.cpu().execute(ostrich::Push{ ostrich::RegisterName::rax });
    ui.render();
    print("dec rbx");

    vm.cpu().execute(ostrich::Dec{ ostrich::RegisterName::rbx });
    ui.render();
    print("DONE");

    std::cin.get();
}