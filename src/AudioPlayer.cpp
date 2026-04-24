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
            case (int)SampleFormat::PCM_U8:  file_format = AUDIO_U8; break;
            case (int)SampleFormat::PCM_S16: file_format = AUDIO_S16SYS; break;
            case (int)SampleFormat::PCM_S32: file_format = AUDIO_S32SYS; break;
        }
    }
    else if(config.audioFormat == 3)
    {
        file_format = AUDIO_F32SYS;
    }
    else{
        throw std::runtime_error("Unsupported format of audio\n");
    }

    spec.format = AUDIO_F32SYS;
    spec.channels = config.numChannels;
    spec.samples = 2048;//размер данных, можно и менять, но это не особо важно нужно разобраться как лучше подбирать
    spec.callback = nullptr;
    spec.userdata = nullptr;
    /**
     * @todo ОБЯЗАТЕЛЬНО ПЕРЕДЕЛАТЬ ЧТОБЫ НЕ ОТКРЫВАТЬ ЗАНОВО НУЖНО ПРОВЕРЯТЬ SPEC И ЕСЛИ ДРУГОЙ - ПЕРЕОТКРЫТЬ ИЛИ ПЕРЕНАСТРОИТЬ
    */
    device = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

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
            convertToFloat(chunk_size);
            SDL_QueueAudio(device, samples.data(), samples.size() * sizeof(float));
            return 1;
        }
        else{
            return 0; 
        }

    }
}

void AudioPlayer::convertToFloat(const uint32_t& chunck_size)
{
    switch (file_format) {
    case AUDIO_U8:     convertU8(chunck_size);
        break;
    case AUDIO_S16SYS: convertS16(chunck_size);
        break;
    case AUDIO_S32SYS: convertS32(chunck_size);
        break;
    case AUDIO_F32SYS: convertF32(chunck_size);
        break;
    default:
        break;
    }
}

void AudioPlayer::convertU8(const uint32_t& chunck_size)
{
    samples.resize(chunck_size);
    for(uint32_t i = 0; i<chunck_size; i++)
    {
        float s = (((float)buffer[i] - 128) / 128.0f) * volume;
        if(s > 1.0f) s = 1.0f;
        if(s < -1.0f) s = -1.0f;
        samples[i] = s;
    }
}

void AudioPlayer::convertS16(const uint32_t& chunck_size)
{
    int16_t* in = reinterpret_cast<int16_t*>(buffer);
    uint32_t count = chunck_size/2;
    samples.resize(count);
    for(uint32_t i = 0; i<count; i++)
    {
        float s = ((float)in[i] / 32768.0f) * volume;
        if(s > 1.0f) s = 1.0f;
        if(s < -1.0f) s = -1.0f;
        samples[i] = s;
    }
}

void AudioPlayer::convertS32(const uint32_t& chunck_size)
{
    int32_t* in = reinterpret_cast<int32_t*>(buffer);
    uint32_t count = chunck_size/4;
    samples.resize(count);

    for(uint32_t i = 0; i<count; i++)
    {
        float s = ((float)in[i]/2147483648.0f) * volume;
        if(s > 1.0f) s = 1.0f;
        if(s < -1.0f) s = -1.0f;
        samples[i] = s;
    }
}

void AudioPlayer::convertF32(const uint32_t& chunck_size)
{
    float* in = reinterpret_cast<float*>(buffer);
    uint32_t count = chunck_size/4;
    samples.resize(count);

    for(uint32_t i = 0; i<count; i++)
    {
        if(*in > 1.0f) *in = 1.0f;
        if(*in < -1.0f) *in = -1.0f;
        samples[i] = in[i] * volume;
    }
}
