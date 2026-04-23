#include "AudioPlayer.hpp"
#include <stdexcept>

AudioPlayer::AudioPlayer()
{
    SDL_Init(SDL_INIT_AUDIO);
    buffer = nullptr;

}

AudioPlayer::~AudioPlayer()
{
    SDL_PauseAudioDevice(device, 1);
    SDL_ClearQueuedAudio(device);
    SDL_CloseAudioDevice(device);
    SDL_Quit();

    delete[] buffer;
    buffer = nullptr;
}

void AudioPlayer::setDevice(const SDL_Config &config)
{
    if(device)
    {
        SDL_PauseAudioDevice(device, 1);
        SDL_ClearQueuedAudio(device);
        SDL_CloseAudioDevice(device);
        SDL_Quit();
    }

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;

    spec.freq = config.sampleRate;

    if(config.audioFormat == 1)
    {
        switch (config.bitsPerSample)
        {
            case (int)WAVSampleFormat::PCM_U8: spec.format = AUDIO_U8; break;
            case (int)WAVSampleFormat::PCM_S16: spec.format = AUDIO_S16SYS; break;
            case (int)WAVSampleFormat::PCM_S32: spec.format = AUDIO_S32SYS; break;
        }
    }
    else if(config.audioFormat == 3)
    {
        spec.format = AUDIO_F32SYS;
    }
    else{
        throw std::runtime_error("Unsupported format of audio\n");
    }
    spec.channels = config.numChannels;
    spec.samples = 2048;//размер данных, можно и менять, но это не особо важно нужно разобраться как лучше подбирать
    spec.callback = nullptr;
    spec.userdata = nullptr;
    /**
     * @todo ОБЯЗАТЕЛЬНО ПЕРЕДЕЛАТЬ ЧТОБЫ НЕ ОТКРЫВАТЬ ЗАНОВО НУЖНО ПРОВЕРЯТЬ SPEC И ЕСЛИ ДРУГОЙ - ПЕРЕОТКРЫТЬ ИЛИ ПЕРЕНАСТРОИТЬ
    */
    device = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);//ОБЯЗАТЕЛЬНО ПЕРЕДЕЛАТЬ ЧТОБЫ НЕ ОТКРЫВАТЬ ЗАНОВО НУЖНО ПРОВЕРЯТЬ SPEC И ЕСЛИ ДРУГОЙ - ПЕРЕОТКРЫТЬ ИЛИ ПЕРЕНАСТРОИТЬ

    //MAX_QUEUE = config.byte_rate / 2;
    MAX_QUEUE = 65536 * config.numChannels * 4;
    buffer = new uint8_t[MAX_QUEUE];
}

void AudioPlayer::setVolume(const float& volume_level)
{
    if(volume_level >= 0 && volume_level <= 1)
    {
       volume = volume_level;
    }
}

void AudioPlayer::prepareBuffer(const SDL_Config& config)
{
    delete[] buffer;
    buffer = nullptr;

    const uint32_t frames = 2048;
    const uint32_t bytesPerSample = config.bitsPerSample / 8;

    MAX_QUEUE = frames * config.numChannels * bytesPerSample;
    buffer = new uint8_t[MAX_QUEUE];
}

void AudioPlayer::play()
{
    SDL_PauseAudioDevice(device, 0);
}

void AudioPlayer::pause()
{
    SDL_PauseAudioDevice(device, 1);
    SDL_ClearQueuedAudio(device);
}

uint32_t AudioPlayer::getSDLQueuedAudio()
{
    return SDL_GetQueuedAudioSize(device);
}

int AudioPlayer::playChunk(IAudioDecoder* format)
{
    if(SDL_GetQueuedAudioSize(device) >= MAX_QUEUE)
    {
        return 1;
    }
    else{
        uint32_t chunk_size = format->readPCM(buffer, MAX_QUEUE);
        if(chunk_size > 0){
            if(volume < 1)
            {
                changeVolume(chunk_size);
            }
            SDL_QueueAudio(device, buffer, chunk_size);
            return 1;
        }
        else{
            return 0; 
        }

    }
}

void AudioPlayer::changeVolume(const uint32_t& chunck_size)
{
    switch (obtained.format) {
    case AUDIO_U8: changeU8(chunck_size);
        break;
    case AUDIO_S16SYS: changeS16(chunck_size);
        break;
    case AUDIO_S32SYS: changeS32(chunck_size);
        break;
    default:
        break;
    }
}

void AudioPlayer::changeU8(const uint32_t& chunck_size)
{
    for(uint32_t i = 0; i<chunck_size; i++)
    {
        int sample = buffer[i];
        sample -= 128;
        sample = (int)(sample * volume);
        sample += 128;

        if(sample < 0) sample = 0;
        if(sample > 255) sample = 255;

        buffer[i] = (uint8_t)sample;
    }
}

void AudioPlayer::changeS16(const uint32_t& chunck_size)
{
    int16_t* samples = reinterpret_cast<int16_t*>(buffer);
    uint32_t count = chunck_size/2;
    for(uint32_t i = 0; i<count; i++)
    {
        samples[i] = (int16_t)(samples[i] * volume);
    }
}

void AudioPlayer::changeS32(const uint32_t& chunck_size)
{
    int32_t* samples = reinterpret_cast<int32_t*>(buffer);
    uint32_t count = chunck_size/4;
    for(uint32_t i = 0; i<count; i++)
    {
        samples[i] = (int32_t)(samples[i] * volume);
    }
}
