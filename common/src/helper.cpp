#include "helper.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <coroutine>
#include <vector>

std::expected<ResolverResult, asio::error_code> resolve_endpoints(
        asio::io_context&      io_context,
        const std::string_view host,
        const std::string_view service)
{
    auto resolver = asio::ip::tcp::resolver(io_context);
    auto ec       = asio::error_code();

    auto result = resolver.resolve(
            std::string(host),
            std::string(service),
            ec);

    if (ec) return std::unexpected(ec);
    return result;
}

asio::error_code connect_to(
        asio::ip::tcp::socket& socket,
        const ResolverResult&  endpoints)
{
    auto ec = asio::error_code();
    asio::connect(socket, endpoints, ec);
    return ec;
}

asio::awaitable<asio::error_code> simple_send(
        asio::ip::tcp::socket& socket,
        const void*            buf,
        const size_t           len)
{
    auto ec = asio::error_code();

    co_await asio::async_write(
            socket,
            asio::buffer(&len, sizeof(len)),
            asio::redirect_error(asio::use_awaitable, ec));
    if (ec) co_return ec;

    co_await asio::async_write(
            socket,
            asio::buffer(buf, len),
            asio::redirect_error(asio::use_awaitable, ec));
    co_return ec;
}

asio::awaitable<asio::error_code> simple_receive(
        asio::ip::tcp::socket&  socket,
        std::vector<std::byte>& buf)
{
    auto ec    = asio::error_code();
    size_t len = 0;

    co_await asio::async_read(
            socket,
            asio::buffer(&len, sizeof(len)),
            asio::redirect_error(asio::use_awaitable, ec));
    if (ec) co_return ec;

    buf.resize(len);

    co_await asio::async_read(
            socket,
            asio::buffer(buf.data(), len),
            asio::redirect_error(asio::use_awaitable, ec));
    co_return ec;
}