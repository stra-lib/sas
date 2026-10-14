#ifndef SAS_ATOMIC_SINGLE_SLOT_HPP_
#define SAS_ATOMIC_SINGLE_SLOT_HPP_

#include <atomic>
#include <utility>


template <typename T>
class AtomicSingleSlot
{
    enum class State
    {
        empty,
        writing,
        full,
        reading
    };

    T                  value_;
    std::atomic<State> state_ = State::empty;

public:
    void set(T value)
    {
        if (auto expected = State::empty; not state_.compare_exchange_strong(
                    expected, State::writing,
                    std::memory_order_acquire,
                    std::memory_order_relaxed))
            return;
        value_ = std::move(value);
        state_.store(State::full, std::memory_order_release);
    }

    [[nodiscard]] bool get(T& value)
    {
        if (auto expected = State::full; not state_.compare_exchange_strong(
                    expected, State::reading,
                    std::memory_order_acquire,
                    std::memory_order_relaxed))
            return false;
        value = std::move(value_);
        state_.store(State::empty, std::memory_order_release);
        return true;
    }
};

#endif //SAS_ATOMIC_SINGLE_SLOT_HPP_
