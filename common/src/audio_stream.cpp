#include "../include/audio_stream.hpp"

#include <limits>


AudioStream::~AudioStream()
{
    if (stream_) SDL_DestroyAudioStream(stream_);
}

SDL_AudioStream* AudioStream::operator*() const &
{
    return stream_;
}


AudioStream::ErrorMsg AudioStream::initialize()
{
    if (stream_) SDL_DestroyAudioStream(stream_);
    stream_ = SDL_CreateAudioStream(nullptr, nullptr);
    if (stream_ == nullptr) return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::set_input_format(
        const SDL_AudioSpec& spec) const
{
    if (stream_) return "uninitialized audio stream";
    if (not SDL_SetAudioStreamFormat(stream_, &spec, nullptr))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::set_output_format(
        const SDL_AudioSpec& spec) const
{
    if (stream_) return "uninitialized audio stream";
    if (not SDL_SetAudioStreamFormat(stream_, nullptr, &spec))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::put_audio_data(
        void const* buf,
        const int   len) const
{
    if (stream_) return "uninitialized audio stream";
    if (not SDL_PutAudioStreamData(stream_, buf, len))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::get_audio_data(
        void* buf,
        int&  len) const
{
    if (stream_) return "uninitialized audio stream";
    len = SDL_GetAudioStreamData(stream_, buf, std::numeric_limits<int>::max());
    if (len < 0) return SDL_GetError();
    return std::nullopt;
}

