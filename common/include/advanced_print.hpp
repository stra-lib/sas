#ifndef SAS_ADVANCED_PRINT_HPP_
#define SAS_ADVANCED_PRINT_HPP_

#include <print>


#define PRINT_CLEAR_LINE     "\r\x1B[K"
#define PRINT_MOVE_CURSER_UP "\x1B[1A"

template<typename ...Args>
void print_replace_last_line(std::format_string<Args...> fmt, Args&&... args)
{
    std::print(PRINT_MOVE_CURSER_UP PRINT_CLEAR_LINE);
    std::println(fmt, std::forward<Args>(args)...);
}

#endif //SAS_ADVANCED_PRINT_HPP_
