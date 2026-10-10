#ifndef SAS_SENDER_HPP_
#define SAS_SENDER_HPP_

#include <string>
#include <string_view>
#include <optional>
#include <asio.hpp>


class Sender
{
    using StrView  = std::string_view;
    using ErrorMsg = std::optional<std::string>;
    using AsioCtx  = asio::io_context;
    using Socket   = asio::ip::tcp::socket;

    AsioCtx ctx     = {};
    Socket  socket  = Socket(ctx);

    ErrorMsg connect_socket(StrView host, StrView service);
    ErrorMsg set_up_audio_capture();

public:
    void run(StrView host, StrView service);
};

#endif //SAS_SENDER_HPP_
