//#ifndef WAVFORMAT_HPP
//#define WAVFORMAT_HPP
#pragma once
#include "SoundSys/IAudioDecoder.hpp"
#include "Core/SDL_Config.hpp"
#include <filesystem>
#include <fstream>
namespace SoundSys{
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
            Core::SDL_Config open();
            uint32_t readPCM(uint8_t* buffer, const uint32_t MAX_QUEUE);
            void setPosition(const uint32_t position);
            uint32_t getChunkSize() const noexcept;
            uint32_t getCurrentPosition() const noexcept;
            void repeat();
    };
}
//#endif
