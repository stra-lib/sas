#include "animation.hpp"

#include <thread>


DotStrAnimation::Frame DotStrAnimation::get() const noexcept
{
    return frames_[current_];
}

DotStrAnimation::Frame DotStrAnimation::next_frame() noexcept
{
    current_ = (current_ + 1) % frames_.size();
    last_    = Clock::now();
    return get();
}

DotStrAnimation::Frame DotStrAnimation::wait_until_next_frame() noexcept
{
    const auto now     = Clock::now();
    const auto elapsed = now - last_;

    if (elapsed < interval_)
        std::this_thread::sleep_for(interval_ - elapsed);

    return next_frame();
}

void DotStrAnimation::reset() noexcept
{
    current_ = 0;
    last_    = Clock::time_point();
}
