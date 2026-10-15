#ifndef SAS_SENDER_HPP_
#define SAS_SENDER_HPP_

#include <asio.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "audio_stream.hpp"
#include "sdl_guard.hpp"
#include "win_capture.hpp"


class Sender
{
    using StrView        = std::string_view;
    using ErrorMsg       = std::optional<std::string>;
    using CoErrMsg       = asio::awaitable<ErrorMsg>;
    using AsioCtx        = asio::io_context;
    using Socket         = asio::ip::tcp::socket;
    using AudioStreamPtr = std::shared_ptr<AudioStream>;

    AsioCtx        ctx_     = {};
    Socket         socket_  = Socket(ctx_);
    SdlGuard       guard_   = {};
    AudioStreamPtr stream_  = std::make_shared<AudioStream>();
    WinCapture     capture_ = {};

    ErrorMsg run_sharing();
    CoErrMsg run_sharing_async();
    ErrorMsg connect_socket(StrView host, StrView service);
    ErrorMsg set_up_audio_capture();

public:
    void run(StrView host, StrView service);
};

#endif //SAS_SENDER_HPP_
