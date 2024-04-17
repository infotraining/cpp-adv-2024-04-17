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
        unique_ptr()
            : p_{nullptr}
        { }

        unique_ptr(nullptr_t)
            : p_{nullptr}
        { }

        explicit unique_ptr(T* ptr)
            : p_{ptr}
        { }

        unique_ptr(const unique_ptr& ptr) = delete;
        unique_ptr& operator=(const unique_ptr& ptr) = delete;

        unique_ptr(unique_ptr&& ptr)
            : p_{std::exchange(ptr.p_, nullptr)}
        { }

        unique_ptr& operator=(unique_ptr&& ptr)
        {
            if (this != &ptr)
            {
                delete p_;
                p_ = std::exchange(ptr.p_, nullptr);
            }

            return *this;
        }

        ~unique_ptr()
        {
            delete p_;
        }

        T* get() const { return p_; }

        explicit operator bool() const
        {
            return p_ != nullptr;
        }

        bool operator==(const unique_ptr& rhs) const
        {
            return p_ == rhs.p_;
        }

        bool operator!=(const unique_ptr& rhs) const
        {
            return !(*this == rhs);
        }

        T* operator->() const { return p_; }

        T& operator*() const { return *p_; }

    private:
        T* p_;
    };
} // namespace Explain

Explain::unique_ptr<Helpers::Gadget> create_gadget(const std::string& name)
{
    static int id_gen = 0;
    const int id = ++id_gen;

    Explain::unique_ptr<Helpers::Gadget> ptr{new Helpers::Gadget(id, name + "#" + std::to_string(id))};
    
    return ptr;
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
        Explain::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};

        if (ptr != nullptr)
        {
            ptr->use();
            (*ptr).use();
        }
    }

    SECTION("copy semantics is disabled")
    {
        Explain::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};
        // Explain::unique_ptr<Gadget> backup = ptr;
    }

    SECTION("move semantics is enabled")
    {
        Explain::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};
        Explain::unique_ptr<Gadget> target = std::move(ptr);
        CHECK_FALSE(ptr);

        target->use();
    }

    SECTION("vector of unique_ptr")
    {
        Explain::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};

        std::vector<Explain::unique_ptr<Gadget>> gadgets;

        gadgets.push_back(Explain::unique_ptr<Gadget>{new Gadget{665, "ipod1"}});
        gadgets.push_back(Explain::unique_ptr<Gadget>{new Gadget{667, "ipod2"}});
        gadgets.push_back(Explain::unique_ptr<Gadget>{new Gadget{669, "ipod3"}});
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
        std::unique_ptr<Gadget> ptr{new Gadget{42, "ipad"}};
        ptr->use();

        std::shared_ptr<Gadget> sptr1 = std::move(ptr);
        std::shared_ptr<Gadget> sptr2 = sptr1;
    }
}