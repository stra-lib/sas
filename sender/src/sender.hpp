#ifndef SAS_SENDER_HPP_
#define SAS_SENDER_HPP_

#include <string>
#include <string_view>
#include <optional>
#include <asio.hpp>
#include "sdl_guard.hpp"
#include "audio_stream.hpp"


class Sender
{
    using StrView  = std::string_view;
    using ErrorMsg = std::optional<std::string>;
    using AsioCtx  = asio::io_context;
    using Socket   = asio::ip::tcp::socket;

    Socket      socket_ = Socket(ctx_);
    AsioCtx     ctx_    = {};
    SdlGuard    guard_  = {};
    AudioStream stream_ = {};

    ErrorMsg connect_socket(StrView host, StrView service);
    ErrorMsg set_up_audio_capture();

public:
    void run(StrView host, StrView service);
};

#endif //SAS_SENDER_HPP_
