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
void deduce3(T&& arg)
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
        deduce1(x);
        deduce1(cx);     
        deduce1(ref_x);  
        deduce1(cref_x); 
        deduce1(tab);    
        deduce1(foo);    

        auto a1 = x;
        auto a2 = cx;
        auto a3 = ref_x;
        auto a4 = cref_x;
        auto a5 = tab;
        auto a6 = foo;
    }

    SECTION("Case 2")
    {
        deduce2(x);      
        deduce2(cx);     
        deduce2(ref_x);  
        deduce2(cref_x); 
        deduce2(tab);    
        deduce2(foo);    

        auto& a1 = x;
        auto& a2 = cx;
        auto& a3 = ref_x;
        auto& a4 = cref_x;
        auto& a5 = tab;
        auto& a6 = foo;
    }

    SECTION("Case 3")
    {
        deduce3(x);              
        deduce3(cx);             
        deduce3(ref_x);          
        deduce3(cref_x);         
        deduce3(tab);            
        deduce3(foo);            
        deduce3(string("text")); 

        auto&& a1 = x;
        auto&& a2 = cx;
        auto&& a3 = ref_x;
        auto&& a4 = cref_x;
        auto&& a5 = tab;
        auto&& a6 = foo;
        auto&& a7 = string("text");
    }
}

TEST_CASE("decltype(auto)")
{ 
}