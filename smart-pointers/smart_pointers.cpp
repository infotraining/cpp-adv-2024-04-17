#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

using Helpers::Gadget;

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void* legacy_foobar()
    {
        void* mem = malloc(1000);
        return mem;
    }
} // namespace LegacyCode

TEST_CASE("legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = new Gadget(13, "ipad");

        use(g);
        // use_gadget(g);  // UB!!! - use after delete
        //  std::cout << g->name() << std::endl; // UB!!! - use after delete
    }

    {
        Gadget* g = get_gadget("ipad");

        use(g);

        // delete g; // UB!!! - second delete
    }
}

/////////////////////////////////////////////////////////////

namespace ModernCpp
{
    // forward declarations
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    void use(std::unique_ptr<Gadget> g);
    void use_gadget(Gadget* g);

    // definitions
    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        return std::make_unique<Gadget>(++id, name);
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g) // Gadget* - non-owning pointer
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
} // namespace ModernCpp

TEST_CASE("dynamic memory management in modern C+")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());

        LegacyCode::use(g.release());
    }

    {
        use_gadget(get_gadget("ipad").get());
    }

    {
        auto g = std::make_unique<Gadget>(13, "ipad");

        use(std::move(g));
    }

    {
        auto g = get_gadget("ipad");

        use(std::move(g));
    }

    {
        std::vector<int> vec;

        std::unique_ptr<void, void (*)(void*)> ptr{LegacyCode::legacy_foobar(), &::free};

        auto free_mem = [](void* ptr) {
            std::cout << "Free mem: " << ptr << "\n";
            free(ptr);
        };
        std::unique_ptr<void, decltype(free_mem)> ptr2{LegacyCode::legacy_foobar(), free_mem};

        void* mem = ptr.get();

        // vec.at(100);
    }
}

TEST_CASE("shared_ptr")
{
    auto ptr1 = std::make_shared<Gadget>(43, "ipad2");
    auto ptr2 = ptr1; // rc == 2
    CHECK(ptr2.use_count() == 2);

    std::weak_ptr<Gadget> wp = ptr1;
    CHECK(ptr1.use_count() == 2);

    ptr1.reset();
    CHECK(ptr1 == nullptr);
    CHECK(ptr2.use_count() == 1);

    {
        std::shared_ptr<Gadget> temp_ptr = wp.lock(); // rc == 2
        if (temp_ptr)
            temp_ptr->use();
    } // rc == 1

    ptr2.reset();

    std::cout << "--------------\n";

    {
        std::shared_ptr<Gadget> temp_ptr = wp.lock();
        CHECK(temp_ptr == nullptr);

        try
        {
            std::shared_ptr<Gadget> temp_ptr(wp);
        }
        catch(const std::bad_weak_ptr& e)
        {
            std::cout << e.what() << "\n";
        }
    } 

}