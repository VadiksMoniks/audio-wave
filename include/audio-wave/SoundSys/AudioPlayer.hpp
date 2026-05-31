//#ifndef AUDIO_PLAYER_HPP
//#define AUDIO_PLAYER_HPP
#pragma once
#include <SDL2/SDL.h>
#include "Core/SDL_Config.hpp"
#include "SoundSys/IAudioDecoder.hpp"
#include <vector>
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
        uint32_t MAX_QUEUE;
        //uint8_t* buffer;
        std::vector<uint8_t> buffer;
        float volume = 1.0;
        SDL_AudioFormat file_format;
        std::vector<float> samples;

        void convertToFloat(const uint32_t chunck_size);
        void convertS16(const uint32_t chunck_size);
        void convertU8(const uint32_t chunck_size);
        void convertS32(const uint32_t chunck_size);
        void convertF32(const uint32_t chunck_size);
        
        public:
            AudioPlayer() noexcept;
            ~AudioPlayer();
            void setDevice(const Core::SDL_Config & config);
            void play();
            void pause();
            void setVolume(const float volume);
            uint32_t getSDLQueuedAudio() const;
            int playChunk(IAudioDecoder* format);
    };
}
//#endif
