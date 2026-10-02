#pragma once

#include "IAudioDecoder.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "Libraries/minimp3_ex.h"
#include "Common/types.h"

namespace SoundSys{

    class MP3Format : public IAudioDecoder
    {
        std::ifstream file;
        mp3dec_ex_t dec{};
        u32 current_position = 0;
        //u8* inner_buffer;
        std::vector<u8>inner_buffer;

        public:
            MP3Format(const std::filesystem::path& path);
            ~MP3Format();
            Core::SDL_Config open();
            u32 readPCM(u8* buffer, const u32 MAX_QUEUE);
            void setPosition(const u32 position);
            u32 getChunkSize() const noexcept;
            u32 getCurrentPosition() const noexcept;
            void repeat();
    };
}
