#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP
#include <SDL2/SDL.h>
#include "SDL_Config.hpp"
#include "IAudioDecoder.hpp"
/**
 * @todo АУДИОВИЗУАЛИЗАТОР (СТОЛБИКОВАЯ ДИАГРАММА)
 */
class PlayerUI;

//constexpr uint32_t AUDIOPLAYER_DELAY = 20;
//constexpr uint32_t DATACHUNK_SIZE = 4096;

enum class WAVSampleFormat{
    PCM_U8 = 8,
    PCM_S16 = 16,
    PCM_S32 = 32,
    FLOAT32 = 32,
};

class AudioPlayer
{
    SDL_AudioSpec obtained;
    SDL_AudioDeviceID device = 0;
    uint32_t MAX_QUEUE;
    uint8_t* buffer;
    float volume = 1.0;

    void changeVolume(const uint32_t& chunck_size);
    void changeS16(const uint32_t& chunck_size);
    void changeU8(const uint32_t& chunck_size);
    void changeS32(const uint32_t& chunck_size);
    
    public:
        AudioPlayer();
        ~AudioPlayer();
        void setDevice(const SDL_Config & config);
        void prepareBuffer(const SDL_Config& config);
        void play();
        void pause();
        void setVolume(const float& volume);
        uint32_t getSDLQueuedAudio();
        //void play(SDL_Config& config, SDL_AudioDeviceID& device, std::ifstream& file, TrackInfo& info, PlayerUI& UI);
        int playChunk(IAudioDecoder* format);
};
#endif
