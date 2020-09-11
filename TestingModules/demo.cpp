//#include <variant>
import std.core;

import Ostrich;


void print(const ostrich::Cpu &cpu)
{
    std::cout << cpu.rax() << ", " << cpu.rbx() << std::endl;
}

int main()
{
    std::cout << "Ostrich demo" << std::endl;
    ostrich::Cpu cpu;
    print(cpu);
    cpu.execute(ostrich::Inc{ ostrich::RegisterName::rax });
    cpu.execute(ostrich::Inc{ ostrich::RegisterName::rbx });
    print(cpu);
    cpu.execute(ostrich::Dec{ ostrich::RegisterName::rbx });
    print(cpu);
}