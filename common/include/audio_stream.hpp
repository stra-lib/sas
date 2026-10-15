#ifndef SAS_AUDIO_STREAM_HPP_
#define SAS_AUDIO_STREAM_HPP_

#include <optional>
#include <expected>
#include <string>
#include <SDL3/SDL_audio.h>


class AudioStream
{
    using ErrorMsg = std::optional<std::string>;
    using ExpInt32 = std::expected<int32_t, std::string>;

    SDL_AudioStream* stream_ = nullptr;

public:
    ~AudioStream();
    SDL_AudioStream* operator*() const &;

    ErrorMsg initialize();
    ErrorMsg set_input_format(const SDL_AudioSpec& spec) const;
    ErrorMsg set_output_format(const SDL_AudioSpec& spec) const;
    ErrorMsg put_audio_data(void const* buf, int len) const;
    ExpInt32 get_audio_data(void* buf, int len) const;
    ExpInt32 get_available_bytes_count() const;
};

#endif //SAS_AUDIO_STREAM_HPP_
