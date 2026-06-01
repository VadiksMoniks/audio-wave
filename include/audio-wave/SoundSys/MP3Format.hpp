//#ifndef MP3FORMAT_H
//#define MP3FORMAT_H
#pragma once
#include "IAudioDecoder.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "Libraries/minimp3_ex.h"
namespace SoundSys{

    class MP3Format : public IAudioDecoder
    {
        std::ifstream file;
        mp3dec_ex_t dec{};
        uint32_t current_position = 0;
        //uint8_t* inner_buffer;
        std::vector<uint8_t>inner_buffer;

        public:
            MP3Format(const std::filesystem::path& path);
            ~MP3Format();
            Core::SDL_Config open();
            uint32_t readPCM(uint8_t* buffer, const uint32_t MAX_QUEUE);
            void setPosition(const uint32_t position);
            uint32_t getChunkSize() const noexcept;
            uint32_t getCurrentPosition() const noexcept;
            void repeat();
    };
}
//#endif
