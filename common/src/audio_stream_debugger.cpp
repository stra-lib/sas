#include "audio_stream_debugger.hpp"

void ASDebugger::debug_loop(std::stop_token&& token)
{
    auto profile = std::shared_ptr<ASDebugProfile>();

    while (not token.stop_requested())
    {
        for (int i = 0; i < profiles_.size(); ++i) // NOLINT(*-loop-convert)
        {
            {
                auto lock = std::lock_guard(mutex_);
                profile = profiles_[i];
            }
            profile->run(stream_);
        }

        // no need for high-precision timing
        std::this_thread::sleep_for(wait_t_);
    }
}

ASDebugger::ASDebugger(AudioStream& stream)
    : stream_(stream)
{
    set_debug_runs_per_sec(1);
}

ASDebugger::~ASDebugger()
{
    stop_debugging();
}

void ASDebugger::set_debug_runs_per_sec(const uint32_t runs_per_sec)
{
    if (runs_per_sec == 0) return;
    using namespace std::chrono_literals;
    wait_t_ = 1'000ms / runs_per_sec;
}

void ASDebugger::start_debugging()
{
    if (thread_.joinable()) stop_debugging();
    if (control_.stop_requested()) control_ = std::stop_source();
    thread_ = std::thread(&ASDebugger::debug_loop, this, control_.get_token());
}

void ASDebugger::stop_debugging()
{
    control_.request_stop();
    if (thread_.joinable()) thread_.join();
}
