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
    {
        items_ = new int[size];
        size_ = size;

        // for(int i = 0; i < size; ++i)
        // {
        //     items_[i] = 0;
        // }
        std::fill(items_, items_ + size_, 0);
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
}

void print(const Vector& vec)
{
    std::cout << "vec: ";
    // for (size_t i = 0; i < vec.size(); ++i)
    // {
    //     std::cout << vec[i] << " ";
    // }
    for(const auto& item : vec)
        std::cout << item << " ";
    std::cout << "\n";
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