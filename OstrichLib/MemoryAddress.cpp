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

    std::string MemoryAddress::toString() const
    {
        std::stringstream ss;
        ss << ostrich::toString(base);

        ss << ostrich::toString(indexOperator) << "("
           << (index ? ostrich::toString(*index) : "nullopt") << "*" << static_cast<int>(scale) << ")";

        ss << ostrich::toString(displacementOperator) << static_cast<int>(displacement);
        return ss.str();
    }

    std::string MemoryAddress::toShortString() const
    {
        std::stringstream ss;
        ss << ostrich::toString(base);
        if(index)
        {
            ss << ostrich::toString(indexOperator) << "("
               << (index ? ostrich::toString(*index) : "nullopt") << "*" << static_cast<int>(scale) << ")";
        }
        if(displacement != 0)
        {
            ss << ostrich::toString(displacementOperator) << static_cast<int>(displacement);
        }
        return ss.str();
    }
} // namespace ostrich