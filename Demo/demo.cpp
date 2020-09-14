import std.core;

import Ostrich;

void print(const std::string_view &s)
{
    std::cout << "\n" << s << std::endl;
}

int main()
{
    std::cout << "Ostrich demo" << std::endl;
    ostrich::Vm vm;
    std::cout << ostrich::visualize(vm);
    print("inc rax");
    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rax });
    std::cout << ostrich::visualize(vm);
    print("inc rbx");
    vm.cpu().execute(ostrich::Inc{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
    print("add rax rbx");
    vm.cpu().execute(
    ostrich::Add{ .destination = ostrich::RegisterName::rax, .source = ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
    print("push rbx");
    vm.cpu().execute(ostrich::Push{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);
    print("push rax");
    vm.cpu().execute(ostrich::Push{ ostrich::RegisterName::rax });
    std::cout << ostrich::visualize(vm);
    print("dec rbx");
    vm.cpu().execute(ostrich::Dec{ ostrich::RegisterName::rbx });
    std::cout << ostrich::visualize(vm);

    ostrich::Stack stack{8, 0xff};
}