#ifndef SAS_RECEIVER_HPP_
#define SAS_RECEIVER_HPP_

#include <asio.hpp>
#include <optional>
#include <string>
#include <string_view>

#include "audio_stream.hpp"
#include "sdl_guard.hpp"


class Receiver
{
    using StrView  = std::string_view;
    using ErrorMsg = std::optional<std::string>;
    using CoErrMsg = asio::awaitable<ErrorMsg>;
    using AsioCtx  = asio::io_context;
    using Acceptor = asio::ip::tcp::acceptor;
    using Socket   = asio::ip::tcp::socket;

    SdlGuard    guard_    = {};
    AudioStream stream_   = {};
    AsioCtx     ctx_      = {};
    Acceptor    acceptor_ = Acceptor(ctx_);

    ErrorMsg set_up_audio_stream();
    ErrorMsg set_up_acceptor(StrView service);
    ErrorMsg run_accept_sharing();
    CoErrMsg run_accept_sharing_async();
    CoErrMsg socket_read_loop(Socket& socket, const bool& stopped) const;

public:
    void run(StrView service);
};

#endif //SAS_RECEIVER_HPP_
