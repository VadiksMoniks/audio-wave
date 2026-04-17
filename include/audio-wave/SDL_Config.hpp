#ifndef SDL_CONFIG_HPP
#define SDL_CONFIG_HPP
#include <cstdint>

struct SDL_Config
{
    uint16_t audioFormat;
    uint32_t sampleRate; 
    uint16_t bitsPerSample;
    uint16_t numChannels;
    uint32_t byte_rate;
};

#endif