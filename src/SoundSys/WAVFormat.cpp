#include "WAVFormat.hpp"
#include <cstring>
namespace SoundSys{
    WAVFormat::WAVFormat(const std::filesystem::path& path)
    {
        file.open(path, std::ios_base::binary);
    }

    WAVFormat::~WAVFormat()
    {
        if(file.is_open())
            file.close();
    }

    Core::SDL_Config WAVFormat::open()
    {
        Core::SDL_Config config = {0};
        char chunkId[4];
        u32 totaDataChunkSize;
        char format[4];
        char subchunkId[4];
        u32 subchunkSize;
        
        u16 blockAlign;
        
        char subchunk2Id[4];
        u32 subchunk2Size;

        this->readChar(chunkId, 4);

        if(std::strncmp(chunkId, "RIFF", 4) != 0)
        {
            if(std::strncmp(chunkId, "RIFX", 4) == 0)
            {
                BigEndianess = true;
            }
            else{
                throw std::runtime_error("Wrong type of file\n");
            }
        }
        this->readU32(totaDataChunkSize);

        this->readChar(format, 4);
        if(std::strncmp(format, "WAVE", 4) != 0)
            throw std::runtime_error("Wrong format of file\n");

        std::streampos eof = file.tellg() + static_cast<std::streamoff>(totaDataChunkSize);
        std::streamoff min_chunk_size = 8;
        //ЧТОБЫ ЦИКЛ ТОЧНО ПРЕКРАЩАЛСЯ, ЕСЛИ ДАННЫХ ОСТАЛОСЬ МЕНЬШЕ ЧЕМ 8(chunkName, chunkSize)
        while(file.tellg() + min_chunk_size < eof)
        {
            this->readChar(subchunkId, 4);
            this->readU32(subchunkSize);
            u32 endOfChunk = file.tellg() + static_cast<std::streamoff>(subchunkSize + subchunkSize % 2);

            if(strncmp(subchunkId, "fmt ", 4) == 0)
            {
                this->readU16(config.audioFormat);
                this->readU16(config.numChannels);
                this->readU32(config.sampleRate);
                this->readU32(config.byte_rate);
                this->readU16(blockAlign);
                this->readU16(config.bitsPerSample);
            }
            else if(strncmp(subchunkId, "data", 4) == 0)
            {
                dataChunk_start_position = file.tellg();
                dataChunk_size = subchunkSize;
                current_position = file.tellg();
            }
            /*else if(std::strncmp(subchunkId, "LIST", 4) == 0)
            {   
                this->readChar(subchunk2Id, 4);//INFO
                //file.seekg(endOfChunk, std::ios_base::beg);
                while(file.tellg() < endOfChunk)
                {
                    this->readChar(subchunk2Id, 4);
                    this->readU32(subchunk2Size);

                    std::streamoff next_chunk = file.tellg() + static_cast<std::streamoff>(subchunk2Size + (subchunk2Size % 2));

                    if(strncmp(subchunk2Id, "INAM", 4) == 0)
                    {
                        info.name.resize(subchunk2Size);
                        this->readChar(info.name.data(), subchunk2Size);
                    }
                    else if(strncmp(subchunk2Id, "IART", 4) == 0)
                    {
                        info.artist.resize(subchunk2Size);
                        this->readChar(info.artist.data(), subchunk2Size);
                    }

                    file.seekg(next_chunk, std::ios_base::beg);
                }
            }*/
            file.seekg(endOfChunk, std::ios_base::beg);
                
        }

        if(dataChunk_size == 0 || dataChunk_start_position == 0 || config.sampleRate == 0)
        {
            throw std::runtime_error("Broken format\n");
        }

        file.seekg(dataChunk_start_position);
        return config;
    }

    void WAVFormat::readChar(char* buffer, u32 size_to_read)
    {
        file.read((char*)buffer, size_to_read);
        if(file.gcount() != size_to_read)
            throw std::runtime_error("Reading error\n");
    }

    //ТУТ СОБИРАЕТЬСЯ ЧИСЛО ИЗ БАЙТ И ПЕРЕВОРАЧИВАЕТЬСЯ, ЕСЛИ ЭТО BIG-ENDIANES
    void WAVFormat::readU16(u16& buffer)
    {
        u8 tmp_buffer[2];
        file.read((char*)tmp_buffer, 2);
        if(file.gcount() != 2)
            throw std::runtime_error("Reading error\n");
        
        if(BigEndianess == true)
        {
            buffer = 
                    (static_cast<u16>(tmp_buffer[0]) << 8) | 
                    (static_cast<u16>(tmp_buffer[1]));
        }
        else{
            buffer = 
                    (static_cast<u16>(tmp_buffer[0])) | 
                    (static_cast<u16>(tmp_buffer[1]) << 8);
        }
    }
    //ТУТ СОБИРАЕТЬСЯ ЧИСЛО ИЗ БАЙТ И ПЕРЕВОРАЧИВАЕТЬСЯ, ЕСЛИ ЭТО BIG-ENDIANES
    void WAVFormat::readU32(u32& buffer)
    {
        u8 tmp_buffer[4];
        file.read((char*)tmp_buffer, 4);
        if(file.gcount() != 4)
            throw std::runtime_error("Reading error\n");

        if(BigEndianess == true)
        {
            buffer = 
                    (static_cast<u32>(tmp_buffer[0]) << 24) |
                    (static_cast<u32>(tmp_buffer[1]) << 16) |
                    (static_cast<u32>(tmp_buffer[2]) << 8)  |
                    (static_cast<u32>(tmp_buffer[3]));
        }
        else{
            buffer = 
                    (static_cast<u32>(tmp_buffer[0]))       |
                    (static_cast<u32>(tmp_buffer[1]) << 8)  |
                    (static_cast<u32>(tmp_buffer[2]) << 16) |
                    (static_cast<u32>(tmp_buffer[3]) << 24);
        }
    }

    u32 WAVFormat::readPCM(u8* buffer, const u32 MAX_QUEUE)
    {
        if(current_position < dataChunk_size)
        {
            if(current_position + MAX_QUEUE < dataChunk_size){
                file.read((char*)buffer, MAX_QUEUE);
            }
            else{
                file.read((char*)buffer, dataChunk_size - current_position);
            }
            current_position += file.gcount();
            return file.gcount();
        }
        else{
            current_position = 0;
            return 0;
        }
    }

    void WAVFormat::setPosition(const u32 position)
    {
        current_position = position;
        file.clear();
        file.seekg(current_position, std::ios_base::beg);
    }

    u32 WAVFormat::getChunkSize() const noexcept
    {
        return dataChunk_size;
    }

    u32 WAVFormat::getCurrentPosition() const noexcept
    {
        return current_position;
    }

    void WAVFormat::repeat()
    {
        file.clear();
        current_position = dataChunk_start_position;
        file.seekg(dataChunk_start_position, std::ios_base::beg);
    }
}
