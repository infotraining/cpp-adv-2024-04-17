#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <list>

using Helpers::Gadget;
using namespace std::literals;

template <typename T>
T maximum(T a, T b)
{
    return (a < b) ? b : a;
}

const char* maximum(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

namespace Traits
{
    template <typename T1, typename T2>
    std::common_type_t<T1, T2> maximum(T1 a, T2 b)
    {
        if (a < b)
            return b;
        else
            return a;
    }
} // namespace Traits

namespace Auto
{
    template <typename T1, typename T2>
    auto maximum(T1 a, T2 b)
    {
        return (a < b) ? b : a;
    }
} // namespace Auto

namespace Exercise
{
    template<typename Iter, typename Value>
    Iter find(Iter begin, Iter end, const Value& value)
    {
        for(auto it = begin; it != end; ++it)
        {
            if(*it == value)
            {
                return it;
            }
        }
        return end;
    }

    template<typename Iter, typename Predicate>
    Iter find_if(Iter begin, Iter end, Predicate predicate)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";

        for(auto it = begin; it != end; ++it)
        {
            if(predicate(*it))
            {
                return it;
            }
        }
        return end;
    }

    template<typename ContainerType>
    void zero(ContainerType& container)
    {
        using T = typename ContainerType::value_type;
       
        for(auto&& element : container)
            element = T{};
    }
} // namespace Exercise

TEST_CASE("function templates")
{
    int x = 10;
    int y = 20;

    CHECK(maximum(x, y) == 20);         // maximum<int>
    CHECK(maximum(3.14, 6.28) == 6.28); // maximum<double>
    CHECK(maximum(static_cast<double>(x), 3.14) == 10.0);
    CHECK(maximum<double>(x, 13.14) == 13.14);

    const char* ctxt1 = "Ola";
    const char* ctxt2 = "Ala";

    CHECK(maximum(ctxt1, ctxt2) == "Ola"s);

    int (*ptr)(int, int) = &maximum<int>;

    CHECK(Traits::maximum(42, 3.14) == 42);
    CHECK(Auto::maximum(42, 3.14) == 42);
}

TEMPLATE_TEST_CASE("find", "[algo]", (std::vector<int>), (std::list<int>))
{
    TestType vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("value in range")
    {
        auto pos = Exercise::find(vec.begin(), vec.end(), 5);
        CHECK(pos != vec.end());
        CHECK(*pos == 5);
    }

    SECTION("value not in range")
    {
        auto pos = Exercise::find(vec.begin(), vec.end(), 24);
        CHECK(pos == vec.end());
    }
}

bool is_even(int x)
{
    return x % 2 == 0;
}

TEMPLATE_TEST_CASE("find_if", "[algo]", (std::vector<int>), (std::list<int>))
{
    SECTION("value in range")
    {
        TestType vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto pos = Exercise::find_if(vec.begin(), vec.end(), [](int x) { return x % 2 == 0;});
        CHECK(pos != vec.end());
        CHECK(*pos == 2);
    }

    SECTION("value not in range")
    {
        TestType vec = {1, 3, 5, 7};

        auto pos = Exercise::find_if(vec.begin(), vec.end(), &is_even);
        CHECK(pos == vec.end());
    }
}

TEST_CASE("zero")
{
    std::vector<int> vec = {1, 2, 3};
    Exercise::zero(vec);
    CHECK(vec == std::vector{0, 0, 0});

    std::list<std::string> lst = {"abc", "def"};
    Exercise::zero(lst);
    CHECK(lst == std::list{""s, ""s});

    std::vector<bool> flags = {1, 0, 0, 1};
    Exercise::zero(flags);
    CHECK((flags == std::vector<bool>{0, 0, 0}));
}

struct X
{
    static int A(int x)
    {
        return x * x;
    }
};

struct Y
{
    struct A
    {
        int value;
 
        A(int x) : value{x}
        {}
    };
};

template <typename T>
auto dependent_name_context(int x)
{
    auto value = typename T::A(x);
 
    return value;
}

TEST_CASE("type dependent names")
{
    auto value1 = X::A(42);

    auto value2 = Y::A(42);
}

TEST_CASE("class templates")
{
    // TODO
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}