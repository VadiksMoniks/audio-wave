//#ifndef IAUDIODECODER_H
//#define IAUDIODECODER_H
#pragma once
#include <cstdint>
#include "Core/SDL_Config.hpp"
namespace SoundSys{
    class IAudioDecoder {
    public:
        IAudioDecoder() = default;
        virtual ~IAudioDecoder() = default;

        IAudioDecoder(const IAudioDecoder&) = delete;
        IAudioDecoder& operator=(const IAudioDecoder&) = delete;

        IAudioDecoder(IAudioDecoder&&) = delete;
        IAudioDecoder& operator=(IAudioDecoder&&) = delete;

        virtual Core::SDL_Config open() = 0;
        virtual uint32_t readPCM(uint8_t* buffer, const uint32_t MAX_QUEUE) = 0;
        virtual void setPosition(const uint32_t position) = 0;
        virtual uint32_t getChunkSize() const noexcept = 0;
        virtual uint32_t getCurrentPosition() const noexcept = 0;
        virtual void repeat() = 0;
    };
}

//#endif
