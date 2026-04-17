#ifndef WAVFORMAT_HPP
#define WAVFORMAT_HPP

#include "IAudioDecoder.hpp"
#include "SDL_Config.hpp"
#include <filesystem>
#include "TrackInfo.hpp"
#include <fstream>

class WAVFormat : public IAudioDecoder
{
    std::ifstream file;
    bool BigEndianess = false;
    uint32_t dataChunk_start_position = 0;
    uint32_t dataChunk_size = 0;
    uint32_t current_position;

    void readChar(char* buffer, uint32_t size_to_read);
    void readU16(uint16_t& buffer);
    void readU32(uint32_t& buffer);
    
    public:
        WAVFormat(const std::filesystem::path& path);
        ~WAVFormat();
        SDL_Config open(TrackInfo& info);
        uint32_t readPCM(uint8_t* buffer, const uint32_t& MAX_QUEUE);
        void setPosition(const uint32_t& position);
        uint32_t getChunkSize();
        uint32_t getCurrentPosition();
        void repeat();
};
#endif
