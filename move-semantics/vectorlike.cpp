#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

class Vector
{
public:
    using iterator = int*;
    using const_iterator = const int*;
    using reference = int&;
    using const_reference = const int&;

    Vector()
        : items_{nullptr}
        , size_{0}
    { }

    explicit Vector(size_t size)
        : items_{new int[size]}
        , size_{size}
    {
        std::fill(items_, items_ + size_, 0);
    }

    Vector(std::initializer_list<int> il)
        : items_{new int[il.size()]}
        , size_{il.size()}
    {
        std::copy(il.begin(), il.end(), items_);
    }

    ~Vector()
    {
        delete[] items_;
    }

    size_t size() const
    {
        return size_;
    }

    reference operator[](size_t index)
    {
        return items_[index];
    }

    const_reference operator[](size_t index) const
    {
        return items_[index];
    }

    bool operator==(const Vector& rhs) const
    {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }

    bool operator!=(const Vector& rhs) const
    {
        return !(*this == rhs);
    }

    iterator begin()
    {
        return items_;
    }

    iterator end()
    {
        return items_ + size_;
    }

    const_iterator begin() const
    {
        return items_;
    }

    const_iterator end() const
    {
        return items_ + size_;
    }

private:
    int* items_ = nullptr;
    size_t size_{};
};

std::ostream& operator<<(std::ostream& out, const Vector& vec)
{
    out << "{ ";
    for (const auto& item : vec)
    {
        out << item << " ";
    }
    out << "}";

    return out;
}

TEST_CASE("Vector")
{
    SECTION("default construction")
    {
        Vector vec;
        std::cout << "Address of vec: " << &vec << "\n";
        std::cout << "sizeof(vec): " << sizeof(vec) << "\n";
        CHECK(vec.size() == 0);
    }

    SECTION("constructor with given size")
    {
        Vector vec(10);

        SECTION("size is set")
        {
            CHECK(vec.size() == 10);
        }

        SECTION("all items are zeroed")
        {
            SECTION("iteration")
            {
                for (size_t i = 0; i < vec.size(); ++i)
                {
                    CHECK(vec[i] == 0);
                }
            }

            SECTION("ranger-based for")
            {
                for (const auto& item : vec)
                {
                    CHECK(item == 0);
                }
            }
        }

        SECTION("implicit")
        {
            Vector vec = Vector(10);
            CHECK(vec.size() == 10);
        }
    }

    SECTION("constructor with initializer_list")
    {
        Vector vec = {1, 2, 3};
        CHECK(vec.size() == 3);

        CHECK(vec == Vector{1, 2, 3});
        CHECK(vec != Vector{1, 2, 3, 4});

        SECTION("construction with () & {}")
        {
            Vector vec1(10);
            CHECK(vec1.size() == 10);

            Vector vec2{10};
            CHECK(vec2.size() == 1);
        }
    }
}

void print(const Vector& vec)
{
    std::cout << "vec: " << vec << "\n";
}

TEST_CASE("Vector - indexing")
{
    Vector vec(3);
    vec[0] = 1;
    vec[1] = 2;
    vec[2] = 3;

    CHECK(vec[0] == 1);
    CHECK(vec[1] == 2);
    CHECK(vec[2] == 3);

    print(vec);
}

TEST_CASE("init with {}")
{
    int x1;
    int x2(665);
    int x3{665};
    int x4 = 10;

    char c1(x2);
    char c2{static_cast<char>(x2)};
}

/////////////////////////////////////////////////////////
// operators

struct X
{
    int value;

    X(int v)
        : value{v}
    { }

    // X operator+(const X& rhs)
    // {
    //     return X{this->value + rhs.value};
    // }

    friend X operator+(const X& a, const X& b)
    {
        return X{a.value + b.value};
    }
};

bool operator==(const X& a, const X& b)
{
    return a.value == b.value;
}

TEST_CASE("operators")
{
    X x1{10};
    X x2{20};

    CHECK(x1 + x2 == 30);
    CHECK(x1 + 10 == 20);
    CHECK(10 + x2 == 30);
}