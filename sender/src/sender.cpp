#include "sender.hpp"

#include <exception>
#include <functional>
#include <future>
#include <print>
#include <algorithm>
#include <array>
#include <chrono>
// ReSharper disable once CppUnusedIncludeDirective
#include <coroutine>
#include <cstddef>

#include "advanced_print.hpp"
#include "animation.hpp"
#include "helper.hpp"


static std::expected<ResolverResult, asio::error_code> try_resolve_endpoints(
        asio::io_context& ctx,
        std::string_view host,
        std::string_view service)
{
    using namespace std::chrono_literals;
    auto animation = DotStrAnimation(400ms);

    auto async_endpoints = std::async(
            std::launch::async,
            resolve_endpoints,
            std::ref(ctx),
            host,
            service);

    print_replace_last_line(
            "resolve {}:{} {}",
            host,
            service,
            animation.get());

    while (not is_done(async_endpoints))
    {
        print_replace_last_line(
                "resolve {}:{} {}",
                host,
                service,
                animation.wait_until_next_frame());
    }

    return async_endpoints.get();
}

static asio::error_code try_connect(
        asio::ip::tcp::socket& socket,
        const ResolverResult&  result,
        std::string_view host,
        std::string_view service)
{
    using namespace std::chrono_literals;
    auto animation = DotStrAnimation(400ms);

    auto async_ec = std::async(
            std::launch::async,
            connect_to,
            std::ref(socket),
            std::ref(result));

    print_replace_last_line(
            "connecting to {}:{} {}",
            host,
            service,
            animation.get());

    while (not is_done(async_ec))
    {
        print_replace_last_line(
                "connecting to {}:{} {}",
                host,
                service,
                animation.wait_until_next_frame());
    }

    return async_ec.get();
}

Sender::ErrorMsg Sender::run_sharing()
{
    auto failed = ErrorMsg();

    asio::co_spawn(
            ctx_,
            run_sharing_async(),
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
                                "run_sharing_async failed with exception: {}",
                                e.what());
                    }
                    catch (...)
                    {
                        failed = "run_sharing_async failed with unknown"
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

Sender::CoErrMsg Sender::run_sharing_async()
{
    // 0.1s * 48'000Hz * 2channel * 4byte = bytes for 100ms
    std::array<std::byte, 38'400> buffer = {};

    auto signal  = asio::signal_set(ctx_, SIGINT, SIGTERM);
    auto timer   = std::make_shared<asio::steady_timer>(ctx_);
    auto stopped = std::make_shared<bool>(false);
    auto error   = ErrorMsg(std::nullopt);
    auto cancel  = [this, stopped, timer]()
    {
        *stopped = true;
        timer->cancel();
        socket_.cancel();
    };

    signal.async_wait([cancel](const std::error_code& ec, int)
    {
        if (ec) return;
        cancel();
    });

    capture_.start_capturing();
    while (not *stopped)
    {
        if ((error = capture_.capturing_failed()))
        {
            cancel();
            break;
        }

        auto available = stream_->get_available_bytes_count();
        if (not available)
        {
            error = std::format(
                    "AudioStream::get_available_bytes_count failed: {}",
                    available.error());
            cancel();
            break;
        }

        if (*available < buffer.size())
        {
            using namespace std::chrono_literals;
            using std::chrono::duration_cast;
            using Ms  = std::chrono::milliseconds;
            auto time = 100ms * (buffer.size() - *available) / buffer.size();
            auto ec   = asio::error_code();

            // prevent short waiting times
            time = std::max(5ms, duration_cast<Ms>(time));

            timer->expires_after(time);
            co_await timer->async_wait(
                    asio::redirect_error(asio::use_awaitable, ec));
            if (ec)
            {
                if (ec == asio::error::operation_aborted and *stopped) break;
                error = std::format("async_wait failed: {}", ec.message());
                cancel();
                break;
            }

            continue;
        }

        const auto read = stream_->get_audio_data(buffer.data(), buffer.size());
        if (not read)
        {
            error = std::format(
                    "AudioStream::get_audio_data failed: {}",
                    read.error());
            cancel();
            break;
        }

        if (const auto ec = co_await simple_send(socket_, buffer.data(), *read))
        {
            if (ec == asio::error::operation_aborted and *stopped) break;
            error = std::format("simple_send failed: {}", ec.message());
            cancel();
            break;
        }
    }
    capture_.stop_capturing();
    signal.cancel();
    if (error) co_return error;

    co_return std::nullopt;
}

Sender::ErrorMsg Sender::connect_socket(StrView host, StrView service)
{
    std::println("connecting to {}:{}", host, service);

    auto endpoints = try_resolve_endpoints(ctx_, host, service);
    if (not endpoints) return endpoints.error().message();

    if (const auto failed = try_connect(socket_, *endpoints, host, service))
        return failed.message();

    print_replace_last_line("finished connecting to {}:{}", host, service);
    return std::nullopt;
}

Sender::ErrorMsg Sender::set_up_audio_capture()
{
    std::println("set up audio capture");

    if (const auto failed = SdlGuard::init(SDL_INIT_AUDIO))
        return std::format("SdlGuard::init failed: {}", *failed);

    if (const auto failed = stream_->initialize())
        return std::format("AudioStream::initialize failed: {}", *failed);

    if (const auto failed = capture_.initialize())
        return std::format("WinCapture::initialize failed: {}", *failed);

    const auto format = capture_.get_format();
    if (not format)
        return std::format(
                "WinCapture::get_format failed: {}",
                format.error());
    if (const auto failed = stream_->set_input_format(*format))
        return std::format(
                "AudioStream::set_input_format failed: {}",
                *failed);

    capture_.set_output(stream_);

    constexpr auto spec = SDL_AudioSpec(SDL_AUDIO_F32LE, 2, 48000);
    if (const auto failed = stream_->set_output_format(spec))
        return std::format(
                "AudioStream::set_output_format failed: {}",
                *failed);

    print_replace_last_line("finished set up audio capture");
    return std::nullopt;
}

void Sender::run(const StrView host, const StrView service)
{
    if (const auto error = connect_socket(host, service))
    {
        std::println("\nerror: {}", *error);
        return;
    }

    if (const auto error = set_up_audio_capture())
    {
        std::println("\nerror: {}", *error);
        return;
    }

    std::println("\nrun system audio sharing");

    if (const auto error = run_sharing())
    {
        std::println("\nerror: {}", *error);
    }
}
