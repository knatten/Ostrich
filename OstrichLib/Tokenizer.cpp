module;
#include <fmt/core.h>

#include <algorithm>
#include <optional>
#include <ostream>
#include <string>
#include <variant>

module Ostrich;

namespace ostrich::tokenizer
{
    void skipSpace(std::string_view &input)
    {
        const auto firstNonSpace =
        std::find_if(input.cbegin(), input.cend(), [](const char c) { return c != ' ' && c != '\t'; });
        input.remove_prefix(std::distance(input.cbegin(), firstNonSpace));
    }

    std::optional<Token> tryTokenizeSingleCharacter(std::string_view &input)
    {
        if(input.empty())
        {
            return std::nullopt;
        }

        switch(input[0])
        {
        case ',':
            input.remove_prefix(1);
            return Comma{};
        case '+':
            input.remove_prefix(1);
            return ArithmeticOperator{ "+" };
        case '-':
            input.remove_prefix(1);
            return ArithmeticOperator{ "-" };
        case '*':
            input.remove_prefix(1);
            return ArithmeticOperator{ "*" };
        case '[':
            input.remove_prefix(1);
            return LeftBracket{};
        case ']':
            input.remove_prefix(1);
            return RightBracket{};
        case '(':
            input.remove_prefix(1);
            return LeftParenthesis{};
        case ')':
            input.remove_prefix(1);
            return RightParenthesis{};
        default:
            return std::nullopt;
        }
    }

    bool isNumberCharacter(char c)
    {
        return isdigit(c) || c == 'x';
    }

    bool isWordCharacter(char c)
    {
        return isalnum(c) || (c == '_');
    }

    std::optional<Number> tryTokenizeNumber(std::string_view &input)
    {
        if(!input.empty() && input[0] == 'x') // Otherwise "x123" would be treated as a number
        {
            return std::nullopt;
        }
        const auto end = std::find_if_not(input.cbegin(), input.cend(), isNumberCharacter);
        if(end == input.cbegin())
        {
            return std::nullopt;
        }
        const Number word{ std::string{ input.cbegin(), end } };
        input.remove_prefix(std::distance(input.cbegin(), end));
        return word;
    }

    std::optional<Word> tryTokenizeWord(std::string_view &input)
    {
        const auto end = std::find_if_not(input.cbegin(), input.cend(), isWordCharacter);
        if(end == input.cbegin())
        {
            return std::nullopt;
        }
        const Word word{ std::string{ input.cbegin(), end } };
        input.remove_prefix(std::distance(input.cbegin(), end));
        return word;
    }

    std::vector<Token> tokenize(std::string_view input)
    {
        std::vector<Token> result;
        while(!input.empty())
        {
            skipSpace(input);
            {
                const auto &token = tryTokenizeSingleCharacter(input);
                if(token)
                {
                    result.push_back(*token);
                    continue;
                }
            }
            {
                const auto &token = tryTokenizeNumber(input);
                if(token)
                {
                    result.push_back(*token);
                    continue;
                }
            }
            {
                const auto &token = tryTokenizeWord(input);
                if(token)
                {
                    result.push_back(*token);
                    continue;
                }
            }
        }
        return result;
    }
} // namespace ostrich::tokenizer
