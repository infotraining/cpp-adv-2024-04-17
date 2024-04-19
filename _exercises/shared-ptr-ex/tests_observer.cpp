#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <memory>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() = default;
};

class Subject
{
    int state_;
    std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

public:
    Subject() : state_(0)
    {
    }

    void register_observer(std::weak_ptr<Observer> observer)
    {
        observers_.insert(observer);
    }

    void unregister_observer(std::weak_ptr<Observer> observer)
    {
        observers_.erase(observer);
    }

    void set_state(int new_state)
    {
        if (state_ != new_state)
        {
            state_ = new_state;
            notify("Changed state on: " + std::to_string(state_));
        }
    }

protected:
    void notify(const std::string& event_args)
    {
        for (auto it = observers_.begin(); it != observers_.end();)
        {
            auto& weak_observer = *it;
            std::shared_ptr<Observer> observer = weak_observer.lock();
            if(observer)
            {
                observer->update(event_args);
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }
};

class ConcreteObserver1 : public Observer
{
public:
    void update(const std::string& event) override
    {
        std::cout << "ConcreteObserver1: " << event << std::endl;
    }
};

class ConcreteObserver2 : public Observer, public std::enable_shared_from_this<ConcreteObserver2>
{
public:
    void update(const std::string& event) override
    {
        std::cout << "ConcreteObserver2: " << event << std::endl;
    }

    void register_me(Subject& subject)
    {
        // subject.register_observer(std::shared_ptr<Observer>(this)); // ERROR - cannot convert this to shared_ptr this way
        subject.register_observer(shared_from_this());
    }
};

TEST_CASE("using observer pattern")
{
    using namespace std;

    Subject s;

    auto o1 = std::make_shared<ConcreteObserver1>();
    s.register_observer(o1);

    {
        auto o2 = std::make_shared<ConcreteObserver2>();
        //s.register_observer(o2);
        o2->register_me(s);

        s.set_state(1);

        o2.reset();

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}
