import std.core;

import Ostrich;

int main()
{
    std::cout << "Ostrich demo" << std::endl;
    ostrich::Vm vm;
    std::cout << ostrich::visualize(vm);
    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rax });
    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
    vm.cpu().execute(
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
    vm.cpu().execute(ostrich::Dec{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
}