#ifndef SAS_AUDIO_STREAM_DEBUGGER_HPP_
#define SAS_AUDIO_STREAM_DEBUGGER_HPP_

#include <concepts>
#include <vector>
#include <memory>
#include <stop_token>
#include <mutex>
#include <chrono>
#include <thread>

#include "audio_stream.hpp"


class ASDebugProfile
{
public:
    virtual ~ASDebugProfile() = 0;
    virtual void run(AudioStream& stream) = 0;
};

template<typename T, typename... Args>
concept ASDebugProfile_T =
        std::derived_from<T, ASDebugProfile> and
        std::constructible_from<T, Args...>;

class ASDebugger
{
    using ProfileCollection = std::vector<std::shared_ptr<ASDebugProfile>>;
    using Duration = std::chrono::milliseconds;

    std::thread       thread_   = {};
    std::stop_source  control_  = {};
    std::mutex        mutex_    = {};
    Duration          wait_t_   = {};
    ProfileCollection profiles_ = {};
    AudioStream&      stream_;

    void debug_loop(std::stop_token&& token);

public:
    explicit ASDebugger(AudioStream& stream);
    ~ASDebugger();

    ASDebugger(const ASDebugger&)            = delete;
    ASDebugger& operator=(const ASDebugger&) = delete;
    ASDebugger(ASDebugger&&)                 = delete;
    ASDebugger& operator=(ASDebugger&&)      = delete;

    void set_debug_runs_per_sec(uint32_t runs_per_sec);
    void start_debugging();
    void stop_debugging();

    template<typename Profile, typename... Args>
    requires ASDebugProfile_T<Profile, Args&&...>
    void add(Args&&... args)
    {
        auto lock = std::lock_guard(mutex_);
        profiles_.emplace_back(
                std::make_shared<Profile>(std::forward<Args>(args)...));
    }
};

#endif //SAS_AUDIO_STREAM_DEBUGGER_HPP_
