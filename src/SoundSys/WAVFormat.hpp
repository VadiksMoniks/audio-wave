#pragma once
\
#include "SoundSys/IAudioDecoder.hpp"
#include "Core/SDL_Config.hpp"
#include <filesystem>
#include <fstream>
#include "Common/types.h"

namespace SoundSys{
    class WAVFormat : public IAudioDecoder
    {
        std::ifstream file;
        bool BigEndianess = false;
        u32 dataChunk_start_position = 0;
        u32 dataChunk_size = 0;
        u32 current_position;

        void readChar(char* buffer, u32 size_to_read);
        void readU16(u16& buffer);
        void readU32(u32& buffer);
        
        public:
            WAVFormat(const std::filesystem::path& path);
            ~WAVFormat();
            Core::SDL_Config open();
            u32 readPCM(u8* buffer, const u32 MAX_QUEUE);
            void setPosition(const u32 position);
            u32 getChunkSize() const noexcept;
            u32 getCurrentPosition() const noexcept;
            void repeat();
    };
}
