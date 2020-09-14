import std.core;

import Ostrich;

int main()
{
    std::cout << "Ostrich demo" << std::endl;
    ostrich::Cpu cpu;
    std::cout << ostrich::visualize(cpu);
    cpu.execute(ostrich::Inc{ ostrich::RegisterName::rax });
    cpu.execute(ostrich::Inc{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(cpu);
    cpu.execute(ostrich::Add{ .destination =ostrich::RegisterName::rax, .source =ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(cpu);
    cpu.execute(ostrich::Dec{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(cpu);
}