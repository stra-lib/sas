#ifndef SAS_HELPER_HPP_
#define SAS_HELPER_HPP_

#include <asio.hpp>
#include <expected>
#include <string_view>
#include <future>
#include <chrono>


template<typename T>
bool is_done(std::future<T>& future)
{
    using namespace std::chrono_literals;
    return future.wait_for(0ms) == std::future_status::ready;
}

using ResolverResult = asio::ip::tcp::resolver::results_type;

std::expected<ResolverResult, asio::error_code> resolve_endpoints(
        asio::io_context& io_context,
        std::string_view  host,
        std::string_view  service);

asio::error_code connect_to(
        asio::ip::tcp::socket& socket,
        const ResolverResult&  endpoints);


#endif //SAS_HELPER_HPP_
