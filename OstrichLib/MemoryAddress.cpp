module;

#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>


module Ostrich;
namespace ostrich
{
    std::string toString(AdditiveOperator additiveOperator)
    {
        switch(additiveOperator)
        {
            using enum AdditiveOperator;
        case plus:
            return "+";
        case minus:
            return "-";
        }
        throw std::runtime_error("No such additive operator");
    }

    std::ostream &operator<<(std::ostream &os, AdditiveOperator additiveOperator)
    {
        os << toString(additiveOperator);
        return os;
    }

    std::string MemoryAddress::toString() const
    {
        std::stringstream ss;
        ss << base;

        ss << indexOperator << "(" << (index ? ostrich::toString(*index) : "nullopt") << "*"
           << static_cast<int>(scale) << ")";

        ss << displacementOperator << static_cast<int>(displacement);
        return ss.str();
    }

    std::string MemoryAddress::toShortString() const
    {
        std::stringstream ss;
        ss << base;
        if(index)
        {
            ss << indexOperator << "(" << (index ? ostrich::toString(*index) : "nullopt") << "*"
               << static_cast<int>(scale) << ")";
        }
        if(displacement != 0)
        {
            ss << displacementOperator << static_cast<int>(displacement);
        }
        return ss.str();
    }
} // namespace ostrich