#ifndef IAUDIODECODER_H
#define IAUDIODECODER_H
#include <cstdint>
#include "SDL_Config.hpp"
#include "TrackInfo.hpp"

class IAudioDecoder {
public:
    IAudioDecoder() = default;
    virtual ~IAudioDecoder() = default;

    IAudioDecoder(const IAudioDecoder&) = delete;
    //IAudioDecoder& operator=(const IAudioDecoder&) = delete;

    virtual SDL_Config open(TrackInfo& info) = 0;
    virtual uint32_t readPCM(uint8_t* buffer, const uint32_t& MAX_QUEUE) = 0;
    virtual void setPosition(const uint32_t& position) = 0;
    virtual uint32_t getChunkSize() = 0;
    virtual uint32_t getCurrentPosition() = 0;
    virtual void repeat() = 0;
};

#endif
