#ifndef SAS_RECEIVER_HPP_
#define SAS_RECEIVER_HPP_

#include <optional>
#include <string>
#include <string_view>


class Receiver
{
    using StrView  = std::string_view;
    using ErrorMsg = std::optional<std::string>;


public:
    void run(StrView service);
};

#endif //SAS_RECEIVER_HPP_
