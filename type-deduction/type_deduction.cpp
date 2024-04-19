#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce2(T& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce3(T&& arg) // universal reference
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("type deduction rules")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = x;
    int tab[10];

    SECTION("Case 1")
    {
        deduce1(x);        // deduce1<T=int>(int arg)
        deduce1(cx);       // deduce1<T=int>(int arg)
        deduce1(ref_x);    // deduce1<T=int>(int arg)
        deduce1(cref_x);   // deduce1<T=int>(int arg)
        deduce1(tab);      // deduce1<T=int*>(int* arg) - decay to pointer
        deduce1(foo);      // deduce1<T=void(*)(int)>(void(*arg)(int))

        auto a1 = x;       // int
        auto a2 = cx;      // int
        auto a3 = ref_x;   // int
        auto a4 = cref_x;  // int
        auto a5 = tab;     // int*
        auto a6 = foo;     // void(*)(int)
    }

    SECTION("Case 2")
    {
        deduce2(x);         // deduce2<T=int>(int& arg) 
        deduce2(cx);        // deduce2<T=const int>(const int& arg)
        deduce2(ref_x);     // deduce2<T=int>(int& arg)
        deduce2(cref_x);    // deduce2<T=const int>(const int& arg)
        deduce2(tab);       // deduce2<T=int[10]>(int(&arg)[10])
        deduce2(foo);       // deduce1<T=void(&)(int)>(void(&arg)(int)) 

        auto& a1 = x;       // int&
        auto& a2 = cx;      // const int&
        auto& a3 = ref_x;   // int&
        auto& a4 = cref_x;  // const int&
        auto& a5 = tab;     // int(&a5)[10]
        auto& a6 = foo;     // void(&)(int)
    }

    SECTION("Case 3")
    {
        deduce3(x);                  // deduce3<T=int&>(int& && -> int&)  
        deduce3(cx);                 // deduce3<T=const int&>(const int& && -> const int&) 
        deduce3(ref_x);              // deduce3<T=int&>(int& && -> int&)
        deduce3(cref_x);             // deduce3<T=const int&>(const int& && -> const int&) 
        deduce3(tab);                // deduce3<T=int(&)[10]>(int(&)[10] && -> int(&)[10])
        deduce3(foo);                // deduce3<T=void(&)(int)>(void(&)(int) && -> void(&)(int)) 
        deduce3(string("text"));     // deduce3<T=std::string>(std::string&&)

        auto&& a1 = x;               // int&
        auto&& a2 = cx;              // const int&
        auto&& a3 = ref_x;           // int&
        auto&& a4 = cref_x;          // const int&  
        auto&& a5 = tab;             // int(&)[10]
        auto&& a6 = foo;             // void(&)(int)
        auto&& a7 = string("text");  // std::string&&
    }
}

namespace Explain
{
    template <typename TContainer>
    auto begin(TContainer& container)
    {
        return container.begin();
    }

    template <typename TContainer>
    auto end(TContainer& container)
    {
        return container.end();
    }

    template <typename T, size_t N>
    auto begin(T (&arr)[N])
    {
        return arr;
    }

    template <typename T, size_t N>
    auto end(T (&arr)[N])
    {
        return arr + N;
    }
}

TEST_CASE("deduction for native arrays")
{
    int vec[] = {1, 2, 3, 4};

    int sum{};
    for(auto it = Explain::begin(vec); it != Explain::end(vec); ++it)
        sum += *it;
    
    CHECK(sum == 10);
}


TEST_CASE("decltype(auto)")
{ 
}