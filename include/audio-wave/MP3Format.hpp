#ifndef MP3FORMAT_H
#define MP3FORMAT_H
#include "IAudioDecoder.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include "minimp3_ex.h"

class MP3Format : public IAudioDecoder
{
    std::ifstream file;
    mp3dec_ex_t dec{};
    uint32_t current_position = 0;
    uint8_t* inner_buffer;
    uint64_t buffer_size;

    public:
        MP3Format(const std::filesystem::path& path);
        ~MP3Format();
        SDL_Config open(TrackInfo& info);
        uint32_t readPCM(uint8_t* buffer, const uint32_t& MAX_QUEUE);
        void setPosition(const uint32_t& position);
        uint32_t getChunkSize();
        uint32_t getCurrentPosition();
        void repeat();
};

#endif
