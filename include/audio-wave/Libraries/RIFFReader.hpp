#pragma once
#include <string>
#include <cstring>
#include <cstdint>

namespace Libraries{
    class RIFFReader
    {
        char* ptr = nullptr;
        bool BigEndianess = false;
        size_t fsize;
        
        void readU16(uint16_t& buffer)
        {    
            if(BigEndianess == true)
            {
                buffer = 
                        (static_cast<uint16_t>(*ptr) << 8) | 
                        (static_cast<uint16_t>(*(ptr + 1)));
            }
            else{
                buffer = 
                        (static_cast<uint16_t>(*ptr)) | 
                        (static_cast<uint16_t>(*(ptr + 1)) << 8);
            }
        }

        void readU32(uint32_t& buffer)
        {
            if(BigEndianess == true)
            {
                buffer = 
                        (static_cast<uint32_t>(*ptr) << 24) |
                        (static_cast<uint32_t>(*(ptr + 1)) << 16) |
                        (static_cast<uint32_t>(*(ptr + 2)) << 8)  |
                        (static_cast<uint32_t>(*(ptr + 3)));
            }
            else{
                buffer = 
                        (static_cast<uint32_t>(*ptr))       |
                        (static_cast<uint32_t>(*(ptr + 1)) << 8)  |
                        (static_cast<uint32_t>(*(ptr + 2)) << 16) |
                        (static_cast<uint32_t>(*(ptr + 3)) << 24);
            }
        }

        public:
            RIFFReader(const size_t& file_size) : fsize(file_size){};

            int read_buf_trackName(void* buffer, std::string& output)
            {
                return read_buf_frame(buffer, output, "INAM");
            }

            int read_buf_artist(void* buffer, std::string& output)
            {
                return read_buf_frame(buffer, output, "IART");
            }

            int read_buf_frame(void* buffer, std::string& output, const char* frame)
            {
                if (!buffer || !frame) return 0;

                ptr = (char*)buffer;
                if (std::strncmp(ptr, "RIFF", 4) != 0)
                {
                    if (std::strncmp(ptr, "RIFX", 4) == 0)
                    {
                        BigEndianess = true;
                    }
                    else {
                        return 0;
                    }
                }
                ptr += 4; // skip "RIFF" / "RIFX"

                uint32_t totaDataChunkSize;
                this->readU32(totaDataChunkSize);
                ptr += 4; // skip size RIFF

                // Вычисляем абсолютный конец RIFF-данных для защиты от вылета
                char* riff_end_pos = (char*)buffer + 8 + totaDataChunkSize;

                if (std::strncmp(ptr, "WAVE", 4) != 0)
                    return 0;

                ptr += 4; // Пропустили "WAVE"

                // Перебираем верхнеуровневые чанки (fmt, data, LIST и т.д.)
                while (ptr < riff_end_pos)
                {
                    char* subchunkID = ptr;
                    ptr+= 4;// skip LIST

                    uint32_t subChunkSize;
                    this->readU32(subChunkSize);
                    ptr += 4;// skip LIST size
                    if (ptr + subChunkSize > riff_end_pos) return 0;

                    if(std::strncmp(subchunkID, "LIST", 4) == 0)
                    {
                        if (subChunkSize < 4) return 0;

                        char* subchunk2ID = ptr;
                        ptr += 4;// skip subframe name

                        if(std::strncmp(subchunk2ID, "INFO", 4) != 0)
                        {
                            ptr += subChunkSize + subChunkSize%2;
                            continue;
                        }

                        char* list_end_pos = ptr + subChunkSize - 4;
                        while(ptr < list_end_pos)
                        {
                            if (ptr + 8 > list_end_pos) break;
                            char* frame_name = ptr;
                            ptr += 4;

                            uint32_t frame_size;
                            this->readU32(frame_size);
                            ptr += 4;

                            if(std::strncmp(frame_name, frame, 4) != 0)
                            {
                                ptr += frame_size + frame_size % 2;
                            }
                            else{
                                output.resize(frame_size);
                                std::memcpy(output.data(), ptr, frame_size);
                                return 1;
                            }
                        }
                    }
                    else{
                        ptr += subChunkSize + subChunkSize%2;
                    }
                }
                return 0;
            }
    };
}
