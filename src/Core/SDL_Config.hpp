#pragma once
#include <cstdint>
#include "Common/types.h"

namespace Core{
    struct SDL_Config
    {
        u16 audioFormat;
        u32 sampleRate; 
        u16 bitsPerSample;
        u16 numChannels;
        u32 byte_rate;
    };
}