#ifndef SAS_SDL_GUARD_HPP_
#define SAS_SDL_GUARD_HPP_

#include <optional>
#include <string>
#include <SDL3/SDL_init.h>


class SdlGuard
{
    using ErrorMsg = std::optional<std::string>;
public:
    ~SdlGuard();
    static ErrorMsg init(SDL_InitFlags flags);
};

#endif //SAS_SDL_GUARD_HPP_
