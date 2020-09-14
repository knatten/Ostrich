module;

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>
module Ostrich;


namespace ostrich
{
    template <class... Ts> struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    std::string toString(RegisterName registerName)
    {
        switch(registerName)
        {
            using enum RegisterName;
        case rax:
            return "rax";
        case rbx:
            return "rbx";
        case rsp:
            return "rsp";
        }
        throw std::runtime_error("No such register");
    }

    std::string Inc::toString() const
    {
        return "inc  " + ostrich::toString(registerName);
    }

    std::string Dec::toString() const
    {
        return "dec  " + ostrich::toString(registerName);
    }

    std::string Add::toString() const
    {
        return "add  " + ostrich::toString(destination) + " " + ostrich::toString(source);
    }

    std::string Push::toString() const
    {
        return "push " + ostrich::toString(registerName);
    }

    Stack::Stack(uint64_t size, uint64_t top) : m_size{ size }, m_top{ top }, m_content(m_size, 0)
    {
    }

    const std::vector<uint8_t> Stack::content() const
    {
        return m_content;
    }

    uint64_t Stack::top() const
    {
        return m_top;
    }

    void Stack::store(uint64_t address, uint64_t value)
    {
        const size_t lsb_index = m_top - address + 7;
        for(uint64_t i = 0; i < 8; ++i)
        {
            const uint8_t byte{ static_cast<uint8_t>(value >> i * 8) };
            const size_t index{ lsb_index - i };
            m_content.at(index) = byte; // TODO bounds check at function level instead
        }
    }

    Cpu::Cpu(Stack &stack) : m_stack(stack), m_rsp{ stack.top() }
    {
    }

    void Cpu::execute(const Instruction &instruction)
    {
        std::visit(overloaded{
                   [this](const Inc &inc) { reg(inc.registerName)++; },
                   [this](const Dec &dec) { reg(dec.registerName)--; },
                   [this](const Add &add) { reg(add.destination) += reg(add.source); },
                   [this](const Push &push) {
                       m_stack.store(m_rsp, reg(push.registerName));
                       m_rsp -= 8;
                   },
                   },
                   instruction);
    }

    uint64_t Cpu::rax() const
    {
        return m_rax;
    }

    uint64_t Cpu::rbx() const
    {
        return m_rbx;
    }

    uint64_t Cpu::rsp() const
    {
        return m_rsp;
    }

    uint64_t &Cpu::reg(RegisterName r)
    {
        switch(r)
        {
            using enum RegisterName;
        case rax:
            return m_rax;
        case rbx:
            return m_rbx;
        case rsp:
            return m_rsp;
        }
        throw std::runtime_error("No such register");
    }

    const Cpu &Vm::cpu() const
    {
        return m_cpu;
    };

    Cpu &Vm::cpu()
    {
        return m_cpu;
    };

    const Stack &Vm::stack() const
    {
        return m_stack;
    };

    const std::vector<Instruction> &Vm::source() const
    {

        return m_source;
    }

    size_t Vm::nextInstruction() const
    {
        return m_nextInstruction;
    }

    void Vm::step()
    {
        m_cpu.execute(m_source[m_nextInstruction]);
        m_nextInstruction++;
    }

    UI::UI(size_t width, size_t height, Vm &vm) : m_width(width), m_height(height), m_vm(vm)
    {
    }

    void UI::render_register(const std::string &name, uint64_t value, char *buf) const
    {
        std::stringstream ss;
        ss << name << ": " << value;
        std::string s = ss.str();
        std::copy(s.c_str(), s.c_str() + s.size(), &(buf[0]) + static_cast<int>(m_width * 0.4));
    }

    void UI::render() const
    {
        std::vector<char> buffer_owner((m_width * m_height) + 1, ' ');
        buffer_owner.at(m_width * m_height) = '\0';
        char *buf = buffer_owner.data();

        // Source
        for(size_t i = 0; i < m_vm.source().size(); ++i)
        {
            const auto &instruction = m_vm.source()[i];
            // TODO these two lines are a mess, should be a function
            const auto s = std::visit([](const auto &i) { return i.toString(); }, instruction);
            const auto s2 = (i == m_vm.nextInstruction() ? "*" : " ") + s;
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
            std::stringstream ss;
            ss << m_vm.stack().top() - i << " " << static_cast<int>(stack[i]);
            const auto s = ss.str();
            std::copy(s.c_str(), s.c_str() + s.size(), &(buf[m_width * i + 100]));
        }
        std::cout << buf;
    }

    void UI::mainLoop()
    {
        while(m_vm.nextInstruction() < m_vm.source().size())
        {
            render();
            std::cin.get();
            m_vm.step();
        }
        render();
    }
} // namespace ostrich