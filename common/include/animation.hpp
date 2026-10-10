#ifndef SAS_ANIMATION_HPP_
#define SAS_ANIMATION_HPP_

#include <chrono>
#include <array>
#include <format>


class DotStrAnimation
{
    template<typename Ty, std::size_t Size>
    using Array     = std::array<Ty, Size>;
    using Frame     = char const* const;
    using Phase     = uint64_t;
    using Clock     = std::chrono::system_clock;
    template<typename Rep, typename Period>
    using Duration  = std::chrono::duration<Rep, Period>;
    using TimePoint = Clock::time_point;

    Array<Frame, 6> frames_   = {
            " ...",
            ". ..",
            ".. .",
            "... ",
            ".. .",
            ". .."
    };
    Clock::duration interval_;
    TimePoint       last_     = {};
    Phase           current_  = 0;

public:
    template<typename Rep, typename Period>
    explicit DotStrAnimation(Duration<Rep, Period> interval);

    Frame get() const noexcept;
    Frame next_frame()                    noexcept;
    Frame wait_until_next_frame()         noexcept;

    template<typename Rep, typename Period>
    void set_interval(Duration<Rep, Period> interval) noexcept;
    void reset()                                      noexcept;
};

template <typename Rep, typename Period>
DotStrAnimation::DotStrAnimation(Duration<Rep, Period> interval)
    :
    interval_(interval)
{
}

template <typename Rep, typename Period>
void DotStrAnimation::set_interval(Duration<Rep, Period> interval) noexcept
{
    interval_ = interval;
}

#endif //SAS_ANIMATION_HPP_
