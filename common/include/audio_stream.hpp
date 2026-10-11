#ifndef SAS_AUDIO_STREAM_HPP_
#define SAS_AUDIO_STREAM_HPP_

#include <optional>
#include <string>
#include <SDL3/SDL_audio.h>


class AudioStream
{
    using ErrorMsg = std::optional<std::string>;

    SDL_AudioStream* stream_ = nullptr;

public:
    ~AudioStream();
    SDL_AudioStream* operator*() const &;

    ErrorMsg initialize();
    ErrorMsg set_input_format(const SDL_AudioSpec& spec) const;
    ErrorMsg set_output_format(const SDL_AudioSpec& spec) const;
    ErrorMsg put_audio_data(void const* buf, int len) const;
    ErrorMsg get_audio_data(void* buf, int& len) const;
    ;
};

#endif //SAS_AUDIO_STREAM_HPP_
