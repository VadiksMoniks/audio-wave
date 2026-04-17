#include "WAVFormat.hpp"
#include <cstring>

WAVFormat::WAVFormat(const std::filesystem::path& path)
{
    file.open(path, std::ios_base::binary);
}

WAVFormat::~WAVFormat()
{
    if(file.is_open())
        file.close();
}

SDL_Config WAVFormat::open(TrackInfo& info)
{
    SDL_Config config = {0};
    char chunkId[4];
    uint32_t totaDatalChunkSize;
    char format[4];
    char subchunkId[4];
    uint32_t subchunkSize;
    
    uint16_t blockAlign;
    
    char subchunk2Id[4];
    uint32_t subchunk2Size;

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
    this->readU32(totaDatalChunkSize);;

    this->readChar(format, 4);
    if(std::strncmp(format, "WAVE", 4) != 0)
        throw std::runtime_error("Wrong format of file\n");

    std::streampos eof = file.tellg() + static_cast<std::streamoff>(totaDatalChunkSize);
    std::streamoff min_chunk_size = 8;
    //ЧТОБЫ ЦИКЛ ТОЧНО ПРЕКРАЩАЛСЯ, ЕСЛИ ДАННЫХ ОСТАЛОСЬ МЕНЬШЕ ЧЕМ 8(chunkName, chunkSize)
    while(file.tellg() + min_chunk_size < eof)
    {
        this->readChar(subchunkId, 4);
        this->readU32(subchunkSize);
        uint32_t endOfChunk = file.tellg() + static_cast<std::streamoff>(subchunkSize + subchunkSize % 2);

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
        else if(std::strncmp(subchunkId, "LIST", 4) == 0)
        {   
            this->readChar(subchunk2Id, 4);

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
        }
        file.seekg(endOfChunk, std::ios_base::beg);
            
    }

    if(dataChunk_size == 0 || dataChunk_start_position == 0 || config.sampleRate == 0)
    {
        throw std::runtime_error("Broken format\n");
    }

    file.seekg(dataChunk_start_position);
    return config;
}

void WAVFormat::readChar(char* buffer, uint32_t size_to_read)
{
    file.read((char*)buffer, size_to_read);
    if(file.gcount() != size_to_read)
        throw std::runtime_error("Reading error\n");
}

//ТУТ СОБИРАЕТЬСЯ ЧИСЛО ИЗ БАЙТ И ПЕРЕВОРАЧИВАЕТЬСЯ, ЕСЛИ ЭТО BIG-ENDIANES
void WAVFormat::readU16(uint16_t& buffer)
{
    uint8_t tmp_buffer[2];
    file.read((char*)tmp_buffer, 2);
    if(file.gcount() != 2)
        throw std::runtime_error("Reading error\n");
    
    if(BigEndianess == true)
    {
        buffer = 
                (static_cast<uint16_t>(tmp_buffer[0]) << 8) | 
                (static_cast<uint16_t>(tmp_buffer[1]));
    }
    else{
        buffer = 
                (static_cast<uint16_t>(tmp_buffer[0])) | 
                (static_cast<uint16_t>(tmp_buffer[1]) << 8);
    }
}
//ТУТ СОБИРАЕТЬСЯ ЧИСЛО ИЗ БАЙТ И ПЕРЕВОРАЧИВАЕТЬСЯ, ЕСЛИ ЭТО BIG-ENDIANES
void WAVFormat::readU32(uint32_t& buffer)
{
    uint8_t tmp_buffer[4];
    file.read((char*)tmp_buffer, 4);
    if(file.gcount() != 4)
        throw std::runtime_error("Reading error\n");

    if(BigEndianess == true)
    {
        buffer = 
                (static_cast<uint32_t>(tmp_buffer[0]) << 24) |
                (static_cast<uint32_t>(tmp_buffer[1]) << 16) |
                (static_cast<uint32_t>(tmp_buffer[2]) << 8)  |
                (static_cast<uint32_t>(tmp_buffer[3]));
    }
    else{
        buffer = 
                (static_cast<uint32_t>(tmp_buffer[0]))       |
                (static_cast<uint32_t>(tmp_buffer[1]) << 8)  |
                (static_cast<uint32_t>(tmp_buffer[2]) << 16) |
                (static_cast<uint32_t>(tmp_buffer[3]) << 24);
    }
}

uint32_t WAVFormat::readPCM(uint8_t* buffer, const uint32_t& MAX_QUEUE)
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

void WAVFormat::setPosition(const uint32_t& position)
{
    current_position = position;
    file.clear();
    file.seekg(current_position, std::ios_base::beg);
}

uint32_t WAVFormat::getChunkSize()
{
    return dataChunk_size;
}

uint32_t WAVFormat::getCurrentPosition()
{
    return current_position;
}

void WAVFormat::repeat()
{
    file.clear();
    current_position = dataChunk_start_position;
    file.seekg(dataChunk_start_position, std::ios_base::beg);
}
