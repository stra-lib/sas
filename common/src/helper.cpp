#include "helper.hpp"

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