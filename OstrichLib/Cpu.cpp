module Ostrich;



namespace ostrich
{
    template <class... Ts> struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    void Cpu::execute(const Instruction &instruction)
    {
        std::visit(overloaded{
                   [this](const Inc &inc) { reg(inc.registerName)++; },
                   [this](const Dec &dec) { reg(dec.registerName)--; },
                   [this](const Add &add) { reg(add.destination) += reg(add.source); },
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

    uint64_t &Cpu::reg(RegisterName r)
    {
        switch(r)
        {
            using enum RegisterName;
        case rax:
            return m_rax;
        case rbx:
            return m_rbx;
        }
        throw std::runtime_error("No such register");
    }

    std::string visualize(const Cpu &cpu)
    {
        std::stringstream ss;
        ss << cpu.rax() << ", " << cpu.rbx() << std::endl;
        return ss.str();
    }
} // namespace ostrich