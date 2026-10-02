#pragma once

#include <cstdint>
#include "Core/SDL_Config.hpp"
#include "Common/types.h"

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
        virtual u32 readPCM(u8* buffer, const u32 MAX_QUEUE) = 0;
        virtual void setPosition(const u32 position) = 0;
        virtual u32 getChunkSize() const noexcept = 0;
        virtual u32 getCurrentPosition() const noexcept = 0;
        virtual void repeat() = 0;
    };
}
