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
    //TODO parse and popuplate in constructor
    vm.m_source.push_back(ostrich::Inc{ ostrich::RegisterName::rax });
    vm.m_source.push_back(ostrich::Inc{ ostrich::RegisterName::rbx });
    vm.m_source.push_back(
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx });
    vm.m_source.push_back(ostrich::Push{ ostrich::RegisterName::rbx });
    vm.m_source.push_back(ostrich::Push{ ostrich::RegisterName::rax });
    vm.m_source.push_back(ostrich::Dec{ ostrich::RegisterName::rbx });
    ostrich::UI ui(120, 30, vm);
    ui.mainLoop();
    std::cin.get();
}