#include "win_capture.hpp"

#include <audioclient.h>
#include <format>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <utility>
#include <cstddef>
#include <vector>
#include <windows.h>


struct WinCapture::WinCaptureInternal
{
    HANDLE               event      = nullptr;
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice*           device     = nullptr;
    IAudioClient*        audio_c    = nullptr;
    IAudioCaptureClient* capture_c  = nullptr;
    WAVEFORMATEX*        format     = nullptr;
    ErrorMsgSlot         error      = {};
};

WinCapture::ErrorMsg WinCapture::capture(uint32_t& packet_size) const
{
    BYTE*      data    = nullptr;
    uint32_t   count   = 0;
    DWORD      flags   = 0;
    const auto shared  = stream_.lock();
    ErrorMsg   failed = std::nullopt;

    if (not shared) return "AudioStream expired";

    if (HRESULT hr = internal_->capture_c->GetBuffer(
            &data,
            &count,
            &flags,
            nullptr,
            nullptr); FAILED(hr))
        return std::format("GetBuffer failed (0x{:08x})", hr);

    if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
    {
        const auto byte_count =
                static_cast<std::size_t>(count) *
                internal_->format->nBlockAlign;

        const std::vector silence(byte_count, BYTE{0});

        failed = shared->put_audio_data(
                silence.data(),
                static_cast<int>(byte_count));
    }
    else if (data != nullptr)
    {
        failed = shared->put_audio_data(
                data,
                static_cast<int>(count) *
                internal_->format->nBlockAlign);
    }

    if (HRESULT hr = internal_->capture_c->ReleaseBuffer(
            count); FAILED(hr))
        return std::format("ReleaseBuffer failed (0x{:08x})", hr);

    if (failed) return std::format("put_audio_data failed: {}", *failed);

    if (HRESULT hr = internal_->capture_c->GetNextPacketSize(
            &packet_size); FAILED(hr))
        return std::format("GetNextPacketSize failed (0x{:08x})", hr);

    return std::nullopt;
}

void WinCapture::capture_loop()
{
    if (HRESULT hr = internal_->audio_c->Start(); FAILED(hr))
    {
        internal_->error.set(
                std::format("IAudioClient::start failed (0x{:08x})", hr));
        running_ = false;
        return;
    }

    while (running_)
    {
        const DWORD wait_result = WaitForSingleObject(
                internal_->event, 100);
        if (wait_result == WAIT_TIMEOUT) continue;
        if (wait_result != WAIT_OBJECT_0)
        {
            internal_->error.set("WaitForSingleObject failed");
            break;
        }

        uint32_t packet_size = 0;
        if (HRESULT hr = internal_->capture_c->GetNextPacketSize(
                &packet_size); FAILED(hr))
        {
            internal_->error.set(
                    std::format("GetNextPacketSize failed (0x{:08x})", hr));
            break;
        }

        while (packet_size != 0)
        {
            if (const auto failed = capture(packet_size))
            {
                internal_->error.set(*failed);
                running_ = false;
                break;
            }
        }
    }

    if (HRESULT hr = internal_->audio_c->Stop(); FAILED(hr))
        internal_->error.set(
                std::format("IAudioClient::stop failed (0x{:08x})", hr));
    running_ = false;
}

WinCapture::WinCapture()
    : internal_(nullptr)
{
}

WinCapture::~WinCapture()
{
    if (running_ || thread_.joinable()) stop_capturing();

    if (internal_)
    {
        if (internal_->format) CoTaskMemFree(internal_->format);
        if (internal_->capture_c) internal_->capture_c->Release();
        if (internal_->audio_c) internal_->audio_c->Release();
        if (internal_->device) internal_->device->Release();
        if (internal_->enumerator) internal_->enumerator->Release();
        CoUninitialize();
    }
}

WinCapture::ErrorMsg WinCapture::initialize()
{
    if (running_ || thread_.joinable()) stop_capturing();

    internal_ = std::make_unique<WinCaptureInternal>();

    if (HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); FAILED(hr))
        return std::format("CoInitializeEx failed (0x{:08x})", hr);

    if (HRESULT hr = CoCreateInstance(
                __uuidof(MMDeviceEnumerator),
                nullptr,
                CLSCTX_ALL,
                __uuidof(IMMDeviceEnumerator),
                reinterpret_cast<void**>(&internal_->enumerator)); FAILED(hr))
        return std::format("CoCreateInstance failed (0x{:08x})", hr);

    if (HRESULT hr = internal_->enumerator->GetDefaultAudioEndpoint(
                eRender,
                eConsole,
                &internal_->device); FAILED(hr))
        return std::format("GetDefaultAudioEndpoint failed (0x{:08x})", hr);

    if (HRESULT hr = internal_->device->Activate(
                __uuidof(IAudioClient),
                CLSCTX_ALL,
                nullptr,
                reinterpret_cast<void**>(&internal_->audio_c)); FAILED(hr))
        return std::format("Activate failed (0x{:08x})", hr);

    if (HRESULT hr = internal_->audio_c->GetMixFormat(
                &internal_->format); FAILED(hr))
        return std::format("GetMixFormat failed (0x{:08x})", hr);

    if (HRESULT hr = internal_->audio_c->Initialize(
                AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_LOOPBACK |
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                0,
                0,
                internal_->format,
                nullptr); FAILED(hr))
        return std::format("Initialize failed (0x{:08x})", hr);

    if (HRESULT hr = internal_->audio_c->GetService(
                __uuidof(IAudioCaptureClient),
                reinterpret_cast<void**>(&internal_->capture_c)); FAILED(hr))
        return std::format("GetService failed (0x{:08x})", hr);

    if (internal_->event = CreateEventW(
                nullptr,
                false,
                false,
                nullptr); internal_->event == nullptr)
        return std::format("CreateEvent failed (nullptr)");

    if (HRESULT hr = internal_->audio_c->SetEventHandle(
                internal_->event); FAILED(hr))
        return std::format("SetEventHandle failed (0x{:08x})", hr);

    return std::nullopt;
}

WinCapture::ErrorMsg WinCapture::capturing_failed() const
{
    if (std::string error_msg; internal_->error.get(error_msg))
        return error_msg;
    return std::nullopt;
}

WinCapture::ExpAudioSpec WinCapture::get_format() const
{
    const auto    format = internal_->format;
    SDL_AudioSpec spec = {};

    spec.channels = static_cast<int>(format->nChannels);
    spec.freq     = static_cast<int>(format->nSamplesPerSec);

    switch (format->wFormatTag)
    {
    case WAVE_FORMAT_PCM:
        switch (format->wBitsPerSample)
        {
        case 8:  spec.format = SDL_AUDIO_U8; break;
        case 16: spec.format = SDL_AUDIO_S16; break;
        case 32: spec.format = SDL_AUDIO_S32; break;
        default:
            return std::unexpected("unknown pcm audio format");
        }
        break;

    case WAVE_FORMAT_IEEE_FLOAT:
        if (format->wBitsPerSample == 32) spec.format = SDL_AUDIO_F32;
        else return std::unexpected("unknown float audio format");
        break;

    case WAVE_FORMAT_EXTENSIBLE:
    {
        const auto* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(format);
        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
        {
            switch (ext->Format.wBitsPerSample)
            {
            case 8: spec.format = SDL_AUDIO_U8; break;
            case 16: spec.format = SDL_AUDIO_S16; break;
            case 32: spec.format = SDL_AUDIO_S32; break;
            default: return std::unexpected("unknown expcm audio format");
            }
        }
        else if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
        {
            switch (ext->Format.wBitsPerSample)
            {
            case 32: spec.format = SDL_AUDIO_F32; break;
            default: return std::unexpected("unknown exfloat audio format");
            }
        }
        else return std::unexpected("unknown exunknown audio format");
        break;
    }

    default: return std::unexpected("unknown audio format");
    }

    return spec;
}

void WinCapture::set_output(AudioStreamPtr stream)
{
    stream_ = std::move(stream);
}

void WinCapture::start_capturing()
{
    if (not internal_ || running_) return;

    running_ = true;
    thread_ = std::thread(&WinCapture::capture_loop, this);
}

void WinCapture::stop_capturing()
{
    if (running_) running_ = false;
    if (thread_.joinable()) thread_.join();
}
