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
    // TODO parse and popuplate in constructor
    ostrich::Vm vm{ ostrich::Source{
    ostrich::Mov{ ostrich::RegisterName::rax, 0xfedcba9876543210 }, ostrich::Push{ ostrich::RegisterName::rax },
    ostrich::Pop{ ostrich::RegisterName::rbx }, ostrich::Mov{ ostrich::RegisterName::rbx, 0x111 },
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx },
    ostrich::Push{ ostrich::RegisterName::rax }, ostrich::Push{ ostrich::RegisterName::rax },
    ostrich::Dec{ ostrich::RegisterName::rbx } } };
    ostrich::UI ui(120, 30, vm);
    ui.mainLoop();
    std::cin.get();
}