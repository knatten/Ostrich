//#include <variant>
import std.core;

import Ostrich;

int main()
{
    std::cout << "Ostrich demo" << std::endl;
    ostrich::Cpu cpu;
    std::cout << cpu.eax() << std::endl;
    cpu.execute(ostrich::Inc{});
    std::cout << cpu.eax() << std::endl;
    cpu.execute(ostrich::Dec{});
    std::cout << cpu.eax() << std::endl;
}