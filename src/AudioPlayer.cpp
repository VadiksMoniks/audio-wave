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
    device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);//ОБЯЗАТЕЛЬНО ПЕРЕДЕЛАТЬ ЧТОБЫ НЕ ОТКРЫВАТЬ ЗАНОВО НУЖНО ПРОВЕРЯТЬ SPEC И ЕСЛИ ДРУГОЙ - ПЕРЕОТКРЫТЬ ИЛИ ПЕРЕНАСТРОИТЬ

    //MAX_QUEUE = config.byte_rate / 2;
    MAX_QUEUE = 65536 * config.numChannels * 4;
    buffer = new uint8_t[MAX_QUEUE];
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
            SDL_QueueAudio(device, buffer, chunk_size);
            return 1;
        }
        else{
            return 0; 
        }

    }
}
