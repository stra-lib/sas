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
    if (not stream_) return "uninitialized audio stream";
    if (not SDL_SetAudioStreamFormat(stream_, &spec, nullptr))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::set_output_format(
        const SDL_AudioSpec& spec) const
{
    if (not stream_) return "uninitialized audio stream";
    if (not SDL_SetAudioStreamFormat(stream_, nullptr, &spec))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ErrorMsg AudioStream::put_audio_data(
        void const* buf,
        const int   len) const
{
    if (not stream_) return "uninitialized audio stream";
    if (not SDL_PutAudioStreamData(stream_, buf, len))
        return SDL_GetError();
    return std::nullopt;
}

AudioStream::ExpInt32 AudioStream::get_audio_data(
        void*     buf,
        const int len) const
{
    if (not stream_) return std::unexpected("uninitialized audio stream");
    int read = SDL_GetAudioStreamData(stream_, buf, len);
    if (read < 0) return std::unexpected(SDL_GetError());
    return read;
}

AudioStream::ExpInt32 AudioStream::get_available_bytes_count() const
{
    if (not stream_) return std::unexpected("uninitialized audio stream");
    int count = SDL_GetAudioStreamAvailable(stream_);
    if (count < 0) return std::unexpected(SDL_GetError());
    return count;
}

