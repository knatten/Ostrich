#include "catch.hpp"

import Ostrich;

using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using namespace ostrich;

TEST_CASE("Initialization happy path")
{
    Stack s{ 10, 0xff };
    CHECK(s.beginning() == 0xff);

    CHECK_THAT(s.content(), Equals(std::vector<uint8_t>(10, 0)));

    Stack zeroSizeIsOk{ 0, 0 };
}

void makeStack(uint64_t size, uint64_t beginning)
{
    Stack s{ size, beginning };
}

TEST_CASE("Stack fits below the start address")
{
    Stack s{ 10, 9 };
    CHECK_THROWS_WITH(makeStack(10, 8), Contains("Stack is 10 big, so beginning must be at least "
                                                 "9. 8 is too small."));
    CHECK_THROWS_WITH(makeStack(10, 0), Contains("Stack is 10 big, so beginning must be at least "
                                                 "9. 0 is too small."));
}

TEST_CASE("Store / load happy path")
{
    Stack s{ 10, 0xff };
    s.store(0xfe, 0xfedcba9876543210);
    CHECK_THAT(s.content(),
               Equals(std::vector<uint8_t>{ 0x00, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x00 }));
    CHECK(0xfedcba9876543210 == s.load(0xfe));
}

TEST_CASE("Stack overflow during store")
{
    Stack s{ 9, 9 };
    s.store(8, 0);
    CHECK_THROWS_WITH(s.store(7, 0), Contains("Stack overflow"));
    CHECK_THROWS_WITH(s.store(0, 0), Contains("Stack overflow"));
}

TEST_CASE("Stack underflow during store")
{
    Stack s{ 9, 9 };
    s.store(9, 0);
    CHECK_THROWS_WITH(s.store(10, 0), Contains("Stack underflow"));
}

TEST_CASE("Stack overflow during load")
{
    Stack s{ 9, 9 };
    s.load(8);
    CHECK_THROWS_WITH(s.load(7), Contains("Stack overflow"));
    CHECK_THROWS_WITH(s.load(0), Contains("Stack overflow"));
}

TEST_CASE("Stack underflow during load")
{
    Stack s{ 9, 9 };
    s.load(9);
    CHECK_THROWS_WITH(s.load(10), Contains("Stack underflow"));
}