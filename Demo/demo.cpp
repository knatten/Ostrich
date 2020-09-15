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
    ostrich::Inc{ ostrich::RegisterName::rax }, ostrich::Inc{ ostrich::RegisterName::rbx },
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx },
    ostrich::Push{ ostrich::RegisterName::rbx }, ostrich::Push{ ostrich::RegisterName::rax },
    ostrich::Dec{ ostrich::RegisterName::rbx } } };
    ostrich::UI ui(120, 30, vm);
    ui.mainLoop();
    std::cin.get();
}