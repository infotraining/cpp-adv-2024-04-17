#include <catch2/catch_test_macros.hpp>

int foo(int x)
{
    return 2 * x;
}

template <int N>
struct Foo
{
    static constexpr int value = 2 * N;
};

//////////////////////////////////////////////////////////

template <typename T>
struct Identity
{
    using type = T;
};

//////////////////////////////////////////////////////////

template <typename T1, typename T2>
struct IsSame
{
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T>
{
    static constexpr bool value = true;
};

// template variable
template <typename T1, typename T2>
constexpr bool IsSame_v = IsSame<T1, T2>::value;

////////////////////////////////////////////////////////////

template <typename T>
struct RemoveRef
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&>
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&&>
{
    using type = T;
};

template <typename T>
using RemoveRef_t = typename RemoveRef<T>::type;

TEST_CASE("meta-functions")
{   
    CHECK(foo(5) == 10);
    static_assert(Foo<5>::value == 10);

    Identity<int>::type x = 42;
}

TEST_CASE("IsSame")
{
    using T1 = int;
    using T2 = int;
    using T3 = double;

    static_assert(IsSame<T1, T2>::value == true);
    static_assert(IsSame<T1, T3>::value == false);
}

TEST_CASE("RemoveRef")
{
    static_assert(IsSame_v<RemoveRef_t<int&>, int>); 
    static_assert(IsSame_v<RemoveRef_t<int&&>, int>); 
    static_assert(IsSame<RemoveRef_t<int>, int>::value); 
}

template <typename T>
struct IsVoid : std::false_type
{
};

template <>
struct IsVoid<void> : std::true_type
{
};

template <typename T>
constexpr bool IsVoid_v = IsVoid<T>::value;

template<typename T>
struct IsPointer : std::false_type
{
};

template<typename T>
struct IsPointer<T*> : std::true_type
{
};

template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

TEST_CASE("IsVoid")
{
    static_assert(IsVoid<int>::value == false);
    static_assert(IsVoid_v<void> == true);
}

TEST_CASE("IsPointer")
{
    static_assert(IsPointer<int>::value == false);
    static_assert(IsPointer_v<int*> == true);
}