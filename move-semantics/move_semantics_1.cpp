#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& ref_name = name;
        ref_name[0] = 'J';
        CHECK(name == "Jan");

        const std::string& ref_full_name = full_name(name, "Kowalski");
        CHECK(ref_full_name == "Jan Kowalski");
        //ref_full_name[0] = 'P';
    }

    SECTION("C++11")
    {
        std::string&& ref_full_name = full_name(name, "Kowalski");
        ref_full_name[0] = 'P';
        CHECK(ref_full_name == "Pan Kowalski");

        // std::string&& ref_name = name; // ERROR - You cannot bind an lvalue to an rvalue reference
    }
}

TEST_CASE("std::move")
{
    std::string str = "abc";

    std::string target = std::move(str); // equivalent to: std::string target = static_cast<std::string&&>(str);

    CHECK(target == "abc");

    str = "new text";
}