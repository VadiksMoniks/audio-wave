#pragma once

#include <SDL2/SDL.h>
#include "Core/SDL_Config.hpp"
#include "SoundSys/IAudioDecoder.hpp"
#include <vector>
#include "Common/types.h"

namespace SoundSys{
    class AudioPlayer
    {
        enum class SampleFormat{
            PCM_U8 = 8,
            PCM_S16 = 16,
            PCM_S32 = 32,
            FLOAT32 = 32,
        };

        SDL_AudioSpec obtained;
        SDL_AudioDeviceID device = 0;
        u32 MAX_QUEUE;
        //u8* buffer;
        std::vector<u8> buffer;
        float volume = 1.0;
        SDL_AudioFormat file_format;
        std::vector<float> samples;

        void convertToFloat(const u32 chunck_size);
        void convertS16(const u32 chunck_size);
        void convertU8(const u32 chunck_size);
        void convertS32(const u32 chunck_size);
        void convertF32(const u32 chunck_size);
        
        public:
            AudioPlayer() noexcept;
            ~AudioPlayer();
            void setDevice(const Core::SDL_Config & config);
            void play();
            void pause();
            void setVolume(const float volume);
            u32 getSDLQueuedAudio() const;
            int playChunk(IAudioDecoder* format);
    };
}
