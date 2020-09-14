module Ostrich;


namespace ostrich
{
    template <class... Ts> struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
            m_content.at(index) = byte;
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

    std::string visualize(const Vm &vm)
    {
        std::stringstream ss;
        const Cpu &cpu = vm.cpu();
        ss << "Registers: rax " << cpu.rax() << ", rbx " << cpu.rbx() << ", rsp " << cpu.rsp() << std::endl;
        ss << "Stack    : ";
        for(const auto &v : vm.stack().content())
        {
            ss << static_cast<int>(v) << " "; // TODO format nicely
        }
        ss << std::endl;
        return ss.str();
    }
} // namespace ostrich