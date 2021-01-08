module;

#include <fmt/core.h>

#include <cstdint>
#include <stdexcept>

module Ostrich;

namespace ostrich
{
    Stack::Stack(uint64_t size, uint64_t beginning) : m_size{ size }, m_beginning{ beginning }, m_content(m_size, 0)
    {
        if(beginning + 1 < size)
        {
            throw std::runtime_error(
            fmt::format("Stack is {} big, so beginning must be at least {}. {} is too small.", size,
                        size - 1, beginning));
        }
    }

    const std::vector<uint8_t> Stack::content() const
    {
        return m_content;
    }

    uint64_t Stack::beginning() const
    {
        return m_beginning;
    }

    void Stack::store(uint64_t address, uint64_t value)
    {
        if(address > m_beginning)
        {
            throw std::runtime_error("Stack underflow!");
        }
        const size_t lsb_index = m_beginning - address + 7;
        if(lsb_index >= m_content.size())
        {
            throw std::runtime_error("Stack overflow! (No, not that website)");
        }
        for(uint64_t i = 0; i < 8; ++i)
        {
            const uint8_t byte{ static_cast<uint8_t>(value >> i * 8) };
            const size_t index{ lsb_index - i };
            m_content.at(index) = byte;
        }
    }

    uint64_t Stack::load(uint64_t address) const
    {
        if(address > m_beginning)
        {
            throw std::runtime_error("Stack underflow!");
        }
        if(m_beginning - address + 8 > m_content.size())
        {
            throw std::runtime_error("Stack overflow! (No, not that website)");
        }
        uint64_t result{ 0 };
        for(uint64_t i = 0; i < 8; ++i)
        {
            const size_t index{ m_beginning - address + i };
            result += m_content.at(index);
            if(i < 7)
            {
                result = result << 8;
            }
        }
        return result;
    }
} // namespace ostrich
