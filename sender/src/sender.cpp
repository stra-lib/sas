#include "sender.hpp"
#include <print>
#include <future>
#include <functional>
#include "helper.hpp"
#include "animation.hpp"
#include "advanced_print.hpp"


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

Sender::ErrorMsg Sender::connect_socket(StrView host, StrView service)
{
    std::println("connecting to {}:{}", host, service);

    auto endpoints = try_resolve_endpoints(ctx, host, service);
    if (not endpoints) return endpoints.error().message();

    auto error = try_connect(socket, *endpoints, host, service);
    if (error) return error.message();

    print_replace_last_line("finished connecting to {}:{}", host, service);
    return std::nullopt;
}

Sender::ErrorMsg Sender::set_up_audio_capture()
{



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
}
