module;

#include <fmt/core.h>

#include <iostream>
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
        render_register("rax", m_vm.cpu().rax(), buf);
        render_register("rbx", m_vm.cpu().rbx(), buf + m_width);
        render_register("rsp", m_vm.cpu().rsp(), buf + m_width * 2);

        // Stack
        const auto &stack = m_vm.stack().content();
        for(size_t i = 0; i < stack.size(); ++i)
        {
            const auto s = fmt::format("{0:04X}: {1:02X}", m_vm.stack().top() - i, stack[i]);
            std::copy(s.c_str(), s.c_str() + s.size(), &(buf[m_width * i + m_width - 14]));
        }
        std::cout << buf;
        std::cout << "(ostrich) ";
    }

    void UI::mainLoop()
    {
        std::string previousCommand;
        while(m_vm.cpu().nextInstruction() < m_vm.source().size())
        {
            std::string command;
            render();
            std::getline(std::cin, command);
            if(!command.empty())
            {
                previousCommand = command;
            }
            if(previousCommand == "s" || previousCommand == "step")
            {
                m_vm.step();
            }
        }
        render();
    }
} // namespace ostrich
