#ifndef SAS_HELPER_HPP_
#define SAS_HELPER_HPP_

#include <asio.hpp>
#include <chrono>
#include <expected>
#include <future>
#include <string_view>
#include <charconv>
#include <system_error>
#include <type_traits>
#include <concepts>
#include <vector>


template<typename T>
bool is_done(std::future<T>& future)
{
    using namespace std::chrono_literals;
    return future.wait_for(0ms) == std::future_status::ready;
}

template<typename T>
requires std::integral<T>
         && (not std::is_same_v<T, bool>)
         && (not std::is_same_v<T, char>)
std::expected<T, std::string> get(std::string_view str)
{
    T value = 0;
    auto [ptr, ec] = std::from_chars(
            str.data(),
            str.data() + str.size(),
            value);

    if (ec == std::errc::invalid_argument)
        return std::unexpected("invalid argument");

    if (ec == std::errc::result_out_of_range)
        return std::unexpected("result out of range");

    if (ptr != str.data() + str.size())
        return std::unexpected("invalid characters");

    return value;
}

using ResolverResult = asio::ip::tcp::resolver::results_type;

std::expected<ResolverResult, asio::error_code> resolve_endpoints(
        asio::io_context& io_context,
        std::string_view  host,
        std::string_view  service);

asio::error_code connect_to(
        asio::ip::tcp::socket& socket,
        const ResolverResult&  endpoints);

asio::awaitable<asio::error_code> simple_send(
        asio::ip::tcp::socket& socket,
        const void* buf,
        size_t      len);

asio::awaitable<asio::error_code> simple_receive(
        asio::ip::tcp::socket&  socket,
        std::vector<std::byte>& buf);

#endif //SAS_HELPER_HPP_
