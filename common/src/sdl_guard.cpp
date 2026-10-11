#include "../include/sdl_guard.hpp"

SdlGuard::~SdlGuard()
{
    SDL_Quit();
}

SdlGuard::ErrorMsg SdlGuard::init(const SDL_InitFlags flags)
{
    if (not SDL_Init(flags)) return SDL_GetError();
    return std::nullopt;
}
