#include <filesystem>
#include <iostream>
#include <string_view>

import Ostrich;

void print(const std::string_view &s)
{
    std::cout << "\n" << s << std::endl;
    std::cin.get();
}

int main(int argc, char *argv[])
{
    try
    {

        std::cout << argc << std::endl;
        ostrich::Vm vm{ argc == 2 ? ostrich::parse(std::filesystem::path(argv[1])) :
                                    ostrich::parse(std::string_view("")), 58 };
        ostrich::UI ui(120, 30, vm);
        ui.mainLoop();
    }
    catch(std::exception &e)
    {
        std::cerr << "Exception " << e.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}