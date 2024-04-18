#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>

#define ENABLE_MOVE_SEMANTICS
#include "helpers.hpp"

using Helpers::String;

struct Container
{
    String name;
    std::vector<String> items;

    // Container(const Helpers::String& name_arg) : name{name_arg}
    // {
    //     std::cout << "Container(lvalue: " << name_arg << ")\n";
    // }

    // // specialized for r-value
    // Container(Helpers::String&& name_arg) : name{std::move(name_arg)}
    // {
    //     std::cout << "Container(rvalue: " << name_arg << ")\n";
    // }

    // compromise version
    // Container(String name_arg, std::vector<String> items_arg)
    //     : name{std::move(name_arg)}
    //     , items{std::move(items_arg)}
    // { }

    // perfect forwarding
    template <typename T1, typename T2>
    Container(T1&& name_arg, T2&& items_arg)
        : name{std::forward<T1>(name_arg)}
        , items{std::forward<T2>(items_arg)}
    {}
};

void print(const Container& container, std::string_view prefix)
{
    std::cout << prefix << ": { " << container.name << "; [ ";
    for (const auto& item : container.items)
    {
        std::cout << item << " ";
    }
    std::cout << "] }\n";
}

// print(c, "container") "container" const char[10] -> string(const char*)

TEST_CASE("passing args")
{
    SECTION("construction")
    {
        std::vector<String> items = {String("abc"), String("def")};
        String::clear_stats();

        SECTION("l-value")
        {
            String name = "data";
            Container c{name, items};
            CHECK(c.name == "data");

            String::print_stats();

            print(c, "container");
        }

        SECTION("r-value")
        {
            Container c("container", std::move(items)); // passing r-value
            CHECK(c.name == "container");

            String::print_stats();
        }
    }
}

TEST_CASE("rule of five")
{
    Container c_source{"source", std::vector<String>{String("abc"), String("def")}};
    String::clear_stats();

    Container c_target = std::move(c_source);
    String::print_stats();
}