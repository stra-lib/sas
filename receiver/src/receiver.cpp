#include "receiver.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <coroutine>
#include <format>
#include <print>
#include <vector>
#include <SDL3/SDL_audio.h>

#include "advanced_print.hpp"
#include "helper.hpp"


Receiver::ErrorMsg Receiver::set_up_audio_stream()
{
    std::println("-- setting up audio stream");

    if (const auto failed = SdlGuard::init(SDL_INIT_AUDIO))
        return std::format("SdlGuard::init failed: {}", *failed);

    if (const auto failed = stream_.initialize())
        return std::format("AudioStream::initialize failed: {}", *failed);

    constexpr auto spec = SDL_AudioSpec(SDL_AUDIO_F32LE, 2, 48000);
    if (const auto failed = stream_.set_input_format(spec))
        return std::format(
                "AudioStream::set_input_format failed: {}",
                *failed);

    const auto devive = SDL_OpenAudioDevice(
            SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
            nullptr);
    if (devive <= 0)
        return std::format(
                "SDL_OpenAudioDevice failed: {}",
                SDL_GetError());

    if (not SDL_BindAudioStream(devive, *stream_))
        return std::format("SDL_BindAudioStream failed: {}", SDL_GetError());

    print_replace_last_line("-- finished setting up audio stream");
    return std::nullopt;
}

Receiver::ErrorMsg Receiver::set_up_acceptor(const StrView service)
{
    std::println("-- setting up acceptor");

    auto ec         = asio::error_code();
    const auto port = get<asio::ip::port_type>(service);

    if (not port) return std::format("service: {}", port.error());

    const auto ip = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), *port);
    if (acceptor_.open(ip.protocol(), ec))
        return std::format("Acceptor::open failed: {}", ec.message());

    if (acceptor_.bind(ip, ec))
        return std::format("Acceptor::bind failed: {}", ec.message());

    if (acceptor_.listen(1, ec))
        return std::format("Acceptor::listen failed: {}", ec.message());

    print_replace_last_line("-- finished setting up acceptor");
    return std::nullopt;
}

Receiver::ErrorMsg Receiver::run_accept_sharing()
{
    auto failed = ErrorMsg();

    asio::co_spawn(
            ctx_,
            run_accept_sharing_async(),
            [&failed](const std::exception_ptr& ex, ErrorMsg result)
            {
                // should be never true -> unexpected programming error
                if (ex)
                {
                    try
                    {
                        std::rethrow_exception(ex);
                    }
                    catch (const std::exception& e)
                    {
                        failed = std::format(
                                "run_accept_sharing_async failed with"
                                " exception: {}",
                                e.what());
                    }
                    catch (...)
                    {
                        failed = "run_accept_sharing_async failed with unknown"
                                " exception";
                    }

                    return;
                }

                failed = std::move(result);
            });

    ctx_.run();
    if (failed) return failed;

    return std::nullopt;
}

Receiver::CoErrMsg Receiver::run_accept_sharing_async()
{
    auto signal        = asio::signal_set(ctx_, SIGINT, SIGTERM);
    auto stopped       = std::make_shared<bool>(false);
    auto socket        = std::make_shared<asio::ip::tcp::socket>(ctx_);
    auto error         = ErrorMsg(std::nullopt);
    auto suppressed_ec = asio::error_code();
    auto cancel        = [this, stopped, socket]()
    {
        *stopped = true;
        auto ec = asio::error_code();
        if (acceptor_.cancel(ec)) {}
        if (socket->cancel(ec)) {}
    };

    signal.async_wait([cancel](const std::error_code& ec, int)
    {
        if (ec) return;
        cancel();
    });

    while (not *stopped)
    {
        auto ec = asio::error_code();
        co_await acceptor_.async_accept(
                *socket,
                asio::redirect_error(asio::use_awaitable, ec));

        if (ec)
        {
            if (ec == asio::error::operation_aborted and *stopped) break;
            error = std::format(
                    "acceptor::async_accept failed: {}",
                    ec.message());
            break;
        }
        if ((error = co_await socket_read_loop(*socket, *stopped)))
        {
            cancel();
            break;
        }
        if (socket->close(suppressed_ec)) {}
    }
    if (signal.cancel(suppressed_ec)) {}
    if (error) co_return error;

    co_return std::nullopt;
}

Receiver::CoErrMsg Receiver::socket_read_loop(
        Socket&     socket,
        const bool& stopped) const
{
    auto ec      = asio::error_code();
    auto buffer  = std::vector<std::byte>();
    while (not stopped)
    {
        if ((ec = co_await simple_receive(socket, buffer)))
        {
            if (ec == asio::error::timed_out
                    or ec == asio::error::eof
                    or ec == asio::error::connection_reset) break;
            if (ec == asio::error::operation_aborted and stopped) break;

            co_return std::format("simple_receive failed: {}", ec.message());
        }

        if (const auto error = stream_.put_audio_data(
                buffer.data(),
                static_cast<int>(buffer.size())))
        {
            co_return std::format(
                    "AudioStream::put_audio_data failed: {}",
                    *error);
        }
    }
    co_return std::nullopt;
}

void Receiver::run(const StrView service)
{
    if (const auto error = set_up_audio_stream())
    {
        std::println("\nerror: {}", *error);
        return;
    }

    if (const auto error = set_up_acceptor(service))
    {
        std::println("\nerror: {}", *error);
        return;
    }

    if (const auto error = run_accept_sharing())
    {
        std::println("\nerror: {}", *error);
    }
}
