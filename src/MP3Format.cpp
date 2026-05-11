#define MINIMP3_IMPLEMENTATION
#define MINIMP3_EX_IMPLEMENTATION
#include "minimp3_ex.h"
#include "MP3Format.hpp"
#include "ID3Reader.hpp"

MP3Format::MP3Format(const std::filesystem::path& path)
{
    //path_to_file = path.string();
    //buffer_size = std::filesystem::file_size(path);
    //inner_buffer = new uint8_t[buffer_size];
    inner_buffer.resize(std::filesystem::file_size(path));

    file.open(path, std::ios_base::binary);

    file.read((char*)inner_buffer.data(), inner_buffer.size());
    file.close();
}

MP3Format::~MP3Format()
{
    //delete[] inner_buffer;
    //inner_buffer = nullptr;
}

SDL_Config MP3Format::open(TrackInfo& info)
{
    ID3Reader reader((void*)inner_buffer.data(), inner_buffer.size());
    if(reader.file_info.id3_format == 2)
    {
        reader.read_buf_trackName((void*)inner_buffer.data(), info.name);
        reader.read_buf_artist((void*)inner_buffer.data(), info.artist);
        reader.read_buf_apic((void*)inner_buffer.data(), info.apic);
    }

    if(mp3dec_ex_open_buf(&dec, inner_buffer.data(), inner_buffer.size(), MP3D_SEEK_TO_SAMPLE))
        throw std::runtime_error("can't read this file");

    if (dec.info.hz <= 0 || dec.info.channels <= 0) throw std::runtime_error("invalid mp3 info");

    SDL_Config config;
    config.audioFormat   = 1;
    config.sampleRate    = dec.info.hz;
    config.bitsPerSample = 16;
    config.numChannels   = dec.info.channels;
    config.byte_rate     = config.sampleRate * config.numChannels * 2;

    return config;

}

uint32_t MP3Format::readPCM(uint8_t* buffer, const uint32_t MAX_QUEUE)
{
    int16_t tmp_buff[MAX_QUEUE/2];
    uint32_t readSamples = mp3dec_ex_read(&dec, tmp_buff, MAX_QUEUE / 2 );
    current_position += readSamples;

    if(readSamples == 0)
    {
        current_position = 0;
        return 0;
    }

    for(int i = 0; i< readSamples; i++)
    {
        buffer[i*2]   = (uint8_t)( (tmp_buff[i]) & 0xFF );
        buffer[i*2+1] = (uint8_t)( (tmp_buff[i] >> 8) & 0xFF );
        /*for(int ch = 0; ch < dec.info.channels; ch++)
        {
            int16_t sample = tmp_buff[i];
            int idx = (i * dec.info.channels + ch) * 2;
            buffer[idx]   = (uint8_t)(sample & 0xFF);
            buffer[idx+1] = (uint8_t)((sample >> 8) & 0xFF);
        }*/
    }

    return readSamples * 2;
}

void MP3Format::setPosition(const uint32_t position_in_bytes)
{
    uint64_t sampleIndex = position_in_bytes / 2;
    // выравниваем по фреймам (чтобы не начать с середины фрейма)
    sampleIndex /= dec.info.channels; 
    sampleIndex *= dec.info.channels;

    mp3dec_ex_seek(&dec, sampleIndex);
    current_position = sampleIndex;
}

uint32_t MP3Format::getChunkSize() const noexcept
{
    return dec.samples * dec.info.channels;
}

uint32_t MP3Format::getCurrentPosition() const noexcept
{
    return current_position * 2;
}

void MP3Format::repeat()
{
    mp3dec_ex_seek(&dec, 0);
    current_position = 0;
}
