#include "gadget.hpp"
#include <memory>

#include <catch2/catch_test_macros.hpp>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

namespace Explain
{
    template <typename T>
    class unique_ptr
    {
    public:
        unique_ptr() noexcept
            : p_{nullptr}
        { }

        unique_ptr(nullptr_t) noexcept
            : p_{nullptr}
        { }

        explicit unique_ptr(T* ptr) noexcept
            : p_{ptr}
        { }

        unique_ptr(const unique_ptr& ptr) = delete;
        unique_ptr& operator=(const unique_ptr& ptr) = delete;

        unique_ptr(unique_ptr&& ptr) noexcept
            : p_{std::exchange(ptr.p_, nullptr)}
        { }

        unique_ptr& operator=(unique_ptr&& ptr) noexcept
        {
            if (this != &ptr)
            {
                delete p_;
                p_ = std::exchange(ptr.p_, nullptr);
            }

            return *this;
        }

        ~unique_ptr() noexcept
        {
            delete p_;
        }

        T* get() const noexcept { return p_; } 

        explicit operator bool() const noexcept
        {
            return p_ != nullptr;
        }

        bool operator==(const unique_ptr& rhs) const noexcept
        {
            return p_ == rhs.p_;
        }

        bool operator!=(const unique_ptr& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        T* operator->() const noexcept { return p_; }

        T& operator*() const { return *p_; } 

    private:
        T* p_;
    };

    // template <typename T>
    // unique_ptr<T> make_unique()
    // {
    //     return unique_ptr<T>{new T()};
    // }

    // template <typename T, typename TArg1>
    // unique_ptr<T> make_unique(TArg1&& arg1)
    // {
    //     return unique_ptr<T>{new T(std::forward<TArg1>(arg1))};
    // }

    // template <typename T, typename TArg1, typename TArg2>
    // unique_ptr<T> make_unique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return unique_ptr<T>{new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2))};
    // }

    template <typename T, typename... TArgs>
    unique_ptr<T> make_unique(TArgs&&... args)
    {
        return unique_ptr<T>{new T(std::forward<TArgs>(args)...)};
    }

} // namespace Explain

Explain::unique_ptr<Helpers::Gadget> create_gadget(const std::string& name)
{
    static int id_gen = 0;
    const int id = ++id_gen;

    return Explain::make_unique<Helpers::Gadget>(id, name + "#" + std::to_string(id));
}

TEST_CASE("move semantics - unique_ptr")
{
    using Helpers::Gadget;

    SECTION("default constructor")
    {
        Explain::unique_ptr<Gadget> ptr;

        CHECK(ptr.get() == nullptr);
        CHECK(ptr == Explain::unique_ptr<Gadget>{});
        CHECK(ptr == nullptr);
    }

    SECTION("automatic destruction")
    {
        Explain::unique_ptr<Gadget> ptr = Explain::make_unique<Gadget>(42, "ipad");

        if (ptr != nullptr)
        {
            ptr->use();
            (*ptr).use();
        }
    }

    SECTION("copy semantics is disabled")
    {
        Explain::unique_ptr<Gadget> ptr = Explain::make_unique<Gadget>(42, "ipad");
        // Explain::unique_ptr<Gadget> backup = ptr;
    }

    SECTION("move semantics is enabled")
    {
        Explain::unique_ptr<Gadget> ptr = Explain::make_unique<Gadget>(42, "ipad");
        Explain::unique_ptr<Gadget> target = std::move(ptr);
        CHECK_FALSE(ptr);

        target->use();
    }

    SECTION("vector of unique_ptr")
    {
        Explain::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};

        std::vector<Explain::unique_ptr<Gadget>> gadgets;

        gadgets.push_back(Explain::make_unique<Gadget>(665, "ipod1"));
        gadgets.push_back(Explain::make_unique<Gadget>(666, "ipod2"));
        gadgets.push_back(Explain::make_unique<Gadget>(667, "ipod3"));
        gadgets.push_back(create_gadget("smartwatch"));
        gadgets.push_back(create_gadget("smartwatch"));
        gadgets.push_back(create_gadget("smartwatch"));
        gadgets.push_back(std::move(ptr));

        for(const auto& g : gadgets)
        {
            if (g)
                g->use();
        }
    }

    SECTION("std")
    {
        std::unique_ptr<Gadget> ptr = std::make_unique<Gadget>(42, "ipad");
        ptr->use();

        std::shared_ptr<Gadget> sptr1 = std::move(ptr);
        std::shared_ptr<Gadget> sptr2 = sptr1;
    }
}


template <typename... Ts>
void foo(Ts... args)
{}

template <typename T>
void bar(std::initializer_list<T> lst)
{}

TEST_CASE("variadic templates vs. initializer_list")
{
    foo(1, 314.4, "text");
    bar({1, 2, 3, 4});
}