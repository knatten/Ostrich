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
    const char *source = "mov rax 0xfedcba9876543210\n"
                         "push rax\n"
                         "pop rbx\n"
                         "mov rbx 0x111\n"
                         "add rax rbx\n"
                         "push rax\n"
                         "dec rbx\n";
    ostrich::Vm vm{ ostrich::parse(source) };
    ostrich::UI ui(120, 30, vm);
    ui.mainLoop();
    std::cin.get();
}