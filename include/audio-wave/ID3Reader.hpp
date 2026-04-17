#ifndef ID3PARSER_HPP
#define ID3PARSER_HPP
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#pragma once
/**
 * @todo СДЕЛАТЬ ЧТЕНИЕ ID3V1 НО ЭТО УСТАРЕЛО ОНО НЕ ОСОБО НУЖНО
 * @todo В КОНСТРУКТОРЕ МОЖЕТ БЫТЬ ПРОБЛЕМА. ЕСЛИ НЕ НАЙДЕТ ID3, ТО ЧТЕНИЕ НЕ ПОЛУЧИТЬСЯ,
 * @todo ТАК ЧТО НУЖНО ПРОВЕРЯТЬ ЛИБО ВНУТРИ МЕТОДОВ id3p_file_info.id3_format, ЛИБО СДЕЛАТЬ МЕТОД, КОТОРЫЙ ВЕРНЕТ ЗНАЧЕНИЕ id3p_file_info.id3_format
 * @todo decode_ID3_data_frame может как-то поменять default чтобы он не возвращал "" или метод read_buf_frame проверял размер строки и возвращал 0
 */
/*
TIT2	Название
TPE1	Исполнитель
TALB	Альбом
TYER	Год (v2.3)
TDRC	Дата (v2.4)
APIC	Обложка
*/

struct id3p_file_info{
    //size_t file_size;
    uint8_t id3_format = 0;
    uint8_t version;
    uint32_t frame_start_position;
    size_t tag_size;
    uint8_t* major_version;
    uint8_t* minor_version;
};

class ID3Reader
{
    //id3p_file_info file_info;

    int  read_buf_frame(void* file_buffer, const char* framename, std::string& output);

    std::string decode_ISO_8859_1(const uint8_t* data_chunk, const size_t& data_chunk_size);
    std::string decode_UTF_16_BOM(const uint8_t* data_chunk, const size_t& data_chunk_size);
    std::string decode_UTF_16_BS(const uint8_t* data_chunk, const size_t& data_chunk_size);
    std::string decode_UTF_8(const uint8_t* data_chunk, const size_t& data_chunk_size );
    std::string decode_ID3_data_frame(const uint8_t& encode_type_byte, const uint8_t* data_chunk, const size_t& data_chunk_size);
    std::string utf_16_to_utf_8(char16_t* data_chunk, const size_t& data_chunk_size);

public:
    id3p_file_info file_info;

    ID3Reader(void* file_buffer, const size_t& file_size);
    ~ID3Reader();
    int read_buf_trackName(void* file_buffer, std::string& output);
    int read_buf_artist(void* file_buffer, std::string& output);
    int read_buf_apic(void* file_buffer, std::string& output);
};
//void read_ID3V2(std::ifstream& file, id3p_file_info& file_info);//for fstream reading
//int  read_trackName(std::ifstream& file, const id3p_file_info& file_info);//for fstream reading
//int  read_artist(std::ifstream& file, const id3p_file_info& file_info);//for fstream reading

ID3Reader::ID3Reader(void* file_buffer, const size_t& file_size)
{
    char* format = (char*)file_buffer;

    if(std::strncmp(format, "ID3", 3) != 0)
    {
        format = (char*)(format + (file_size - 129));

        if(std::strncmp(format, "TAG", 3) == 0)
        {
            file_info.id3_format = 1;
        }
    }
    else{
        file_info.id3_format = 2;
    }

    uint8_t* ptr = (uint8_t*)file_buffer;
    uint8_t* buff;
    size_t index = 3;
    uint32_t tag_size = 0;

    file_info.major_version = (uint8_t*)(ptr + index);
    index++;
    file_info.minor_version = (uint8_t*)(ptr + index);
    index++;
    index++;//flags ignore
    buff = (uint8_t*)(ptr + index);
    index+=4;

    file_info.frame_start_position = index;
    file_info.tag_size = ( (buff[0] & 0x7F) << 21 ) | ( (buff[1] & 0x7F) << 14 ) | ( (buff[2] & 0x7F) << 7 ) | ( (buff[3] & 0x7F) );
}

ID3Reader::~ID3Reader(){}
//id3v1_info read_buf_ID3V1(void* file_buffer, const id3p_file_info& file_info);

/*id3v1_info read_buf_ID3V1(void* file_buffer, const id3p_file_info& file_info)
{
    //file.seekg(file_info.file_size - 131, std::ios_base::beg);

    id3v1_info info;
    file.read((char*)info.title, 30);
    info.title[30] = '\0';
    file.read((char*)info.artist, 30);
    info.artist[30] = '\0';
    file.read((char*)info.album, 30);
    info.album[30] = '\0';
    file.read((char*)info.year, 4);
    info.year[4] = '\0';

    return info;
}*/

int ID3Reader::read_buf_frame(void* file_buffer, const char* framename, std::string& output)
{
    uint8_t* ptr = (uint8_t*)file_buffer + file_info.frame_start_position;
    uint32_t tag_end = 10 + file_info.tag_size;
    uint8_t* buff;
    uint32_t index = 0;
    while(index < tag_end)
    {
        if (index + 10 > tag_end)
            break;

        char* frame_name = (char*)(ptr + index);
        index+=4;
        buff = (uint8_t*)(ptr + index);
        index+=4;
        uint32_t frame_size;

        if(*file_info.major_version == 3)
            frame_size = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
        else if(*file_info.major_version == 4)
            frame_size = ((buff[0] & 0x7F) << 21) | ((buff[1] & 0x7F) << 14) | ((buff[2] & 0x7F) << 7) | (buff[3] & 0x7F);

        if(frame_size == 0)
            continue;

        //skip flags
        int16_t flags = (buff[4] << 8) | (buff[5]);
        index+=2;

        if(std::strncmp(frame_name, framename, 4) == 0)
        {
            uint8_t* encode_type = (uint8_t*)(ptr + index);
            index+=1;
            uint8_t* data_chunk = (uint8_t*)(ptr + index);
            //index += (frame_size - 1);
            output = decode_ID3_data_frame(*encode_type, data_chunk, frame_size);
            return 1;
        }
        else{
            index +=frame_size;
        }
    }
    return 0;
}

int ID3Reader::read_buf_trackName(void* file_buffer, std::string& output)
{
    return read_buf_frame(file_buffer, "TIT2", output);
}

int ID3Reader::read_buf_artist(void* file_buffer, std::string& output)
{
    return read_buf_frame(file_buffer, "TPE1", output);
}

int ID3Reader::read_buf_apic(void* file_buffer, std::string& output)
{
    uint8_t* ptr = (uint8_t*)file_buffer + file_info.frame_start_position;
    uint32_t tag_end = 10 + file_info.tag_size;
    uint8_t* buff;
    uint32_t index = 0;
    while(index < tag_end)
    {
        if (index + 10 > tag_end)
            break;

        char* frame_name = (char*)(ptr + index);
        index+=4;
        buff = (uint8_t*)(ptr + index);
        index+=4;
        uint32_t frame_size;

        if(*file_info.major_version == 3)
            frame_size = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
        else if(*file_info.major_version == 4)
            frame_size = ((buff[0] & 0x7F) << 21) | ((buff[1] & 0x7F) << 14) | ((buff[2] & 0x7F) << 7) | (buff[3] & 0x7F);

        if(frame_size == 0)
            continue;

        int16_t flags = (buff[4] << 8) | (buff[5]);
        index+=2;

        if (flags & 0x0001) {
            index += 4;
        }

        if(strncmp(frame_name, "APIC", 4) == 0)
        {
            uint32_t data_start_index = index;
            uint8_t encoding = ptr[index];//encoding
            index++;

            while(index < tag_end && (char)ptr[index] != '\0')
            {
                index++;//skip MIME
            }

            index++;//skip \0
            index++;//skip pic type
            if(encoding == 0x00 || encoding == 0x03)
            {
                //uint16_t start = index;
                while((char)ptr[index] != '\0')
                {
                    index++;//skip description
                }
                index++;//skip \0
            }
            else if(encoding == 0x01 || encoding == 0x02){
                //uint16_t start = index;
                while( (uint16_t)( (ptr[index] << 8) | (ptr[index+1]) ) != 0x0000)
                {
                    index+=2;//skip description
                }
                index+=2;//skip 00 00
            }
            //raw bytes start
            uint32_t headers_len = index - data_start_index;
            uint32_t image_data_size = frame_size - headers_len;

            output.resize(image_data_size);
            std::memcpy(output.data(), &ptr[index], image_data_size);
            return 1;
        }
        else{
            index +=frame_size;
        }
    }
    return 0;
}


std::string ID3Reader::decode_ID3_data_frame(const uint8_t& encode_type_byte, const uint8_t* data_chunk, const size_t& data_chunk_size)
{
    switch (encode_type_byte)
    {
    case 0x00: return decode_ISO_8859_1(data_chunk, data_chunk_size);
        break;
    case 0x01: return decode_UTF_16_BOM(data_chunk, data_chunk_size);
        break;
    case 0x02: return decode_UTF_16_BS(data_chunk, data_chunk_size);
        break;
    case 0x03: return decode_UTF_8(data_chunk, data_chunk_size);
        break;
    default:   return "";
        break;
    }
}

std::string ID3Reader::decode_ISO_8859_1(const uint8_t* data_chunk, const size_t& data_chunk_size)
{
    std::string _output;
    _output.resize(data_chunk_size * 2);
    char* ptr = _output.data();

    for(size_t i = 0; i<data_chunk_size; i++)
    {
        if(data_chunk[i] == 0x00)
            break;

        if(data_chunk[i] <= 0x7F)
        {
            *ptr = (char)data_chunk[i];
            ptr++;
        }
        else{
            *ptr = (char)(0xC0 | data_chunk[i] >> 6);
            ptr++;
            *ptr = (char)(0x80 | ( data_chunk[i] & 0x3F));
            ptr++;
        }
    }
    _output.resize(ptr - _output.data());
    return _output;
}

std::string ID3Reader::decode_UTF_16_BOM(const uint8_t* data_chunk, const size_t& data_chunk_size)
{
    if (data_chunk_size < 2) return "";

    bool big_endian = true;
    size_t start = 0;

    if (data_chunk[0] == 0xFE && data_chunk[1] == 0xFF) {
        big_endian = true;
        start = 2;
    } else if (data_chunk[0] == 0xFF && data_chunk[1] == 0xFE) {
        big_endian = false;
        start = 2;
    }

    std::vector<char16_t> row_bytes;
    for (size_t i = start; i + 1 < data_chunk_size; i += 2) {
        uint16_t word;
        if (big_endian)
            word = (data_chunk[i] << 8) | data_chunk[i + 1];
        else
            word = (data_chunk[i + 1] << 8) | data_chunk[i];

        if (word == 0x0000) break;
        row_bytes.push_back(static_cast<char16_t>(word));
    }
    return utf_16_to_utf_8(row_bytes.data(), row_bytes.size());
}

std::string ID3Reader::decode_UTF_16_BS(const uint8_t* data_chunk, const size_t& data_chunk_size)
{
    std::vector<char16_t> row_bytes;
    for(size_t i = 0; i<data_chunk_size; i+=2)
    {
        if(data_chunk[i] == 0x00 && data_chunk[i+1] == 0x00)
            break;

        row_bytes.push_back(static_cast<char16_t>(( data_chunk[i] << 8 ) | ( data_chunk[i+1] )));
    }
    return utf_16_to_utf_8(row_bytes.data(), row_bytes.size());
}

std::string ID3Reader::decode_UTF_8(const uint8_t* data_chunk, const size_t& data_chunk_size)
{
    size_t end = 0;
    while (end < data_chunk_size && data_chunk[end] != 0x00)
        end++;

    std::string _output;
    _output.resize(end);
    std::memcpy(_output.data(), data_chunk, end);
    return _output;
}

std::string ID3Reader::utf_16_to_utf_8(char16_t* data_chunk, const size_t& data_chunk_size)
{
    std::string _output;
    _output.resize(data_chunk_size * 3);
    char* ptr = _output.data();
    uint8_t byte;
    size_t i = 0;

    while(i < data_chunk_size)
    {
        if (data_chunk[i] == 0x0000) break;

        if(data_chunk[i] <= 0x7F)
        {
            *ptr = (char)data_chunk[i];
            ptr ++;
        }
        else{
            if((data_chunk[i] >= 0x80) && (data_chunk[i] <= 0x7FF)){
                *ptr = (char)( 0xC0 | ( data_chunk[i] >> 6 ));
                ptr ++;
                *ptr = (char)0x80 | (data_chunk[i] & 0x3F);
                ptr ++;
            }
            else{
                *ptr = (char)(0xE0 | data_chunk[i] >> 12);
                ptr ++;
                *ptr = (char)(0x80 | (data_chunk[i] >> 6 & 0x3F));
                ptr ++;
                *ptr = (char)(0x80 | (data_chunk[i] & 0x3F));
                ptr ++;
            }
        }
        i++;
    }
    _output.resize(ptr - _output.data());
    return _output;
}

#endif
