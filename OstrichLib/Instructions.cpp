module;

#include <string>
module Ostrich;


namespace ostrich
{
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

} // namespace ostrich
