module;

#include <fmt/core.h>

#include <iostream>
#include <ranges>
#include <string>
#include <variant>
module Ostrich;


namespace ostrich
{

    UI::UI(size_t width, size_t height, Vm &vm) : m_width(width), m_height(height), m_vm(vm)
    {
    }

    void UI::render_register(const std::string &name, uint64_t value, char *buf) const
    {
        const auto s = fmt::format("{0}: {1:016X}", name, value);
        std::copy(s.c_str(), s.c_str() + s.size(), &(buf[0]) + static_cast<int>(m_width * 0.4));
    }

    void UI::render() const
    {
        std::vector<char> buffer_owner((m_width * (m_height - 1)) + 1, ' ');
        buffer_owner.at(m_width * (m_height - 1)) = '\0';
        char *buf = buffer_owner.data();

        // Source
        for(size_t i = 0; i < m_vm.source().size(); ++i)
        {
            const auto &instruction = m_vm.source()[i];
            // TODO these two lines are a mess, should be a function
            const auto s = std::visit([](const auto &i) { return i.toString(); }, instruction);
            const auto s2 = (i == m_vm.cpu().nextInstruction() ? "*" : " ") + s;
            std::copy(s2.c_str(), s2.c_str() + s2.size(), &(buf[m_width * i + 5]));
        }

        // Registers
        for(size_t i = 0; i < m_vm.cpu().registers().size(); ++i)
        {
            const auto &reg = m_vm.cpu().registers()[i];
            render_register(toString(reg.registerName), reg.value, buf + m_width * i);
        }

        // Stack
        const auto &stack = m_vm.stack().content();
        const size_t maxHeight{ m_height - 1 };
        for(size_t i = 0; i < stack.size(); ++i)
        {
            const auto stackPointer{ m_vm.stack().beginning() - i == m_vm.cpu().registerValue(RegisterName::rsp) ? '>' : ' ' };
            const auto s =
            fmt::format("{0}{1:04X}: {2:02X}", stackPointer, m_vm.stack().beginning() - i, stack[i]);
            const size_t row{ i % maxHeight };
            const size_t col{ i / maxHeight };
            std::copy(s.c_str(), s.c_str() + s.size(), &(buf[m_width * row + m_width - 26 + col * 12]));
        }
        std::cout << buf;
        std::cout << "(ostrich) ";
    }

    void UI::mainLoop()
    {
        std::string previousCommand;
        while(true)
        {
            std::string command;
            render();
            std::getline(std::cin, command);
            try
            {
                if(command.empty())
                {
                    if(!previousCommand.empty())
                    {
                        command = previousCommand;
                    }
                    else
                    {
                        continue;
                    }
                }

                if(command == "s" || command == "step")
                {
                    m_vm.step();
                }
                else if(command == "q" || command == "quit")
                {
                    return;
                }
                else if(command.starts_with("load"))
                {
                    m_vm.load(parse(std::filesystem::path(split(command, ' ')[1])));
                }
                else if(command.starts_with("'"))
                {
                    m_vm.execute(parseInstruction(command.substr(1)));
                }
                else if(command == "b" || command == "back")
                {
                    m_vm.restorePreviousState();
                }
                else
                {
                    throw std::runtime_error(fmt::format("Syntax error: '{}'", command));
                }
                previousCommand = command;
            }
            catch(const std::exception &e)
            {
                std::cout << e.what() << std::endl;
                previousCommand = "";
                std::cin.get();
            }
        }
    }
} // namespace ostrich
