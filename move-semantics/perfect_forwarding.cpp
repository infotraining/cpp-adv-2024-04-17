#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Helpers;

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

void have_fun(const Gadget& cg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    cg.use();
}

void have_fun(Gadget&& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

// template <typename TGadget>
// void have_fun(TGadget&& g)
// {
//     std::cout << __PRETTY_FUNCTION__ << "\n";
//     g.use();
// }

namespace WithoutPerfectForwarding
{
    void use(Gadget& g)
    {
        have_fun(g);
    }

    void use(const Gadget& g)
    {
        have_fun(g);
    }

    void use(Gadget&& g)
    {
        have_fun(std::move(g));
    }
} // namespace WithoutPerfectForwarding

namespace PerfectForwarding
{
    template <typename TGadget> // TGadget - deduced as Gadget& for l-value; Gadget for rvalue
    void use(TGadget&& g) // TGadget&& - universal reference
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        have_fun(std::forward<TGadget>(g));

        // if constexpr (std::is_reference_v<TGadget>)
        //     have_fun(g);
        // else
        //     have_fun(std::move(g));
    }
}

TEST_CASE("using gadget")
{
    Gadget g{1, "g"};
    const Gadget cg{2, "const g"};

    using WithoutPerfectForwarding::use;
    std::cout << "\n\n--------------------\n\n";
    use(g);    
    std::cout << "\n\n--------------------\n\n";
    use(cg);
    std::cout << "\n\n--------------------\n\n";
    use(Gadget{3, "temporary gadget"});
}

TEST_CASE("universal reference - auto&&")
{
    auto&& a1 = 42; // int&&

    int x = 10;
    auto&& a2 = x;  // int&


    auto generic_lambda = [](auto&& item) { 
        std::cout << __PRETTY_FUNCTION__ << "\n";
        std::cout << "Item: " << item << "\n";
    };

    generic_lambda(x);
    generic_lambda(42);
}


template <typename T>
void collapse(T& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("ref collapsing")
{
    int x = 10;
    collapse<int>(x);

    collapse<int&>(x);

    collapse<int&&>(x);
}

struct Lambda_567345623786587
{
    std::vector<std::string>& vec;

    Lambda_567345623786587(std::vector<std::string>& vec) : vec{vec}
    {        
    }

    template <typename T>
    auto operator()(T&& item) const
    {
        vec.push_back(std::forward<decltype(item)>(item));
    }
};

TEST_CASE("auto&& in lambda")
{
    std::vector<std::string> vec;

    auto pusher = [&vec](auto&& item) {
        vec.push_back(std::forward<decltype(item)>(item));
    };

    pusher("one");
    pusher("two");
    pusher("three");

    const std::string word = "four";
    pusher(word);
}