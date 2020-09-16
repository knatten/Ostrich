module;

#include <cstdint>

module Ostrich;

namespace ostrich
{
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

    uint64_t Stack::load(uint64_t address)
    {
        uint64_t result{ 0 };
        for(uint64_t i = 0; i < 8; ++i)
        {
            const size_t index{ m_top - address + i };
            // TODO bounds check at function level instead
            result += m_content.at(index);
            if(i < 7)
            {
                result = result << 8;
            }
        }
        return result;
    }
} // namespace ostrich
