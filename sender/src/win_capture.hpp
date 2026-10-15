#ifndef SAS_WIN_CAPTURE_HPP_
#define SAS_WIN_CAPTURE_HPP_

#include <atomic>
#include <expected>
#include <optional>
#include <string>
#include <thread>
#include <SDL3/SDL_audio.h>

#include "atomic_single_slot.hpp"
#include "audio_stream.hpp"


class WinCapture
{
    // Hide OS Internal Requirements
    struct WinCaptureInternal;

    using WinDataPtr     = std::unique_ptr<WinCaptureInternal>;
    using AudioStreamPtr = std::weak_ptr<AudioStream>;
    using ExpAudioSpec   = std::expected<SDL_AudioSpec, std::string>;
    using ErrorMsgSlot   = AtomicSingleSlot<std::string>;
    using ErrorMsg       = std::optional<std::string>;

    std::thread      thread_  = {};
    std::atomic_bool running_ = false;
    AudioStreamPtr   stream_  = {};
    WinDataPtr       internal_;

    ErrorMsg capture(uint32_t& packet_size) const;
    void     capture_loop();

public:
    WinCapture();
    ~WinCapture();

    ErrorMsg     initialize();
    ErrorMsg     capturing_failed() const;
    ExpAudioSpec get_format() const;
    void         set_output(AudioStreamPtr stream);
    void         start_capturing();
    void         stop_capturing();
};

#endif //SAS_WIN_CAPTURE_HPP_
