#include "catch.hpp"

#include <optional>
#include <ostream>
#include <sstream>
#include <variant>

import Ostrich;

using namespace ostrich::tokenizer;
using Catch::Equals;
using tokens = std::vector<Token>;


namespace ostrich::tokenizer
{

    std::ostream &operator<<(std::ostream &os, const tokens &tokens)
    {
        for(const auto &token : tokens)
        {
            std::visit([&os](const auto &t) { os << t << ", "; }, token);
        }
        return os;
    }
} // namespace ostrich::tokenizer

template <typename T>
std::string streamOut(T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

TEST_CASE("Token printing")
{
    CHECK(streamOut(Word{ "foo" }) == std::string{ "Word{foo}" });
    CHECK(streamOut(Number{ "3" }) == std::string{ "Number{3}" });
    CHECK(streamOut(Comma{}) == std::string{ "Comma{,}" });
    CHECK(streamOut(LeftBracket{}) == std::string{ "LeftBracket{[}" });
    CHECK(streamOut(RightBracket{}) == std::string{ "RightBracket{]}" });
    CHECK(streamOut(LeftParenthesis{}) == std::string{ "LeftParenthesis{(}" });
    CHECK(streamOut(RightParenthesis{}) == std::string{ "RightParenthesis{)}" });
}

TEST_CASE("Token equality")
{
    CHECK(Word{ "a" } == Word{ "a" });
    CHECK(Word{ "a" } != Word{ "b" });
    CHECK(Word{ "1" } != Number{ "1" });
    CHECK(Number{ "1" } == Number{ "1" });
}

TEST_CASE("Empty input")
{
    CHECK(tokenize("") == tokens{});
}

TEST_CASE("Single word")
{
    CHECK(tokenize("ret") == tokens{ Word{ "ret" } });
    CHECK(tokenize("leave") == tokens{ Word{ "leave" } });
    CHECK(tokenize("x123") == tokens{ Word{ "x123" } });
}

TEST_CASE("Single tokens")
{
    CHECK(tokenize(",") == tokens{ Comma{} });
    CHECK(tokenize("[") == tokens{ LeftBracket{} });
    CHECK(tokenize("]") == tokens{ RightBracket{} });
    CHECK(tokenize("(") == tokens{ LeftParenthesis{} });
    CHECK(tokenize(")") == tokens{ RightParenthesis{} });
    CHECK(tokenize("+") == tokens{ ArithmeticOperator{ "+" } });
    CHECK(tokenize("-") == tokens{ ArithmeticOperator{ "-" } });
    CHECK(tokenize("*") == tokens{ ArithmeticOperator{ "*" } });
}

TEST_CASE("Single number")
{
    CHECK(tokenize("23") == tokens{ Number{ "23" } });
    CHECK(tokenize("0x123") == tokens{ Number{ "0x123" } });
}


TEST_CASE("Full instructions")
{
    CHECK(tokenize("mov rax, 0x12") == tokens{ Word{ "mov" }, Word{ "rax" }, Comma{}, Number{ "0x12" } });
    CHECK(tokenize("add rbx, [rcx + (rax * 8) - 4 ]") ==
          tokens{ Word{ "add" }, Word{ "rbx" }, Comma{}, LeftBracket{}, Word{ "rcx" }, ArithmeticOperator{ "+" },
                  LeftParenthesis{}, Word{ "rax" }, ArithmeticOperator{ "*" }, Number{ "8" },
                  RightParenthesis{}, ArithmeticOperator{ "-" }, Number{ "4" }, RightBracket{} });
}

TEST_CASE("Leading and trailing spaces")
{
    CHECK(tokenize("\t   foo\t\t  ") == tokens{ Word{ "foo" } });
}
