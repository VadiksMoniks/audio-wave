#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "Common/types.h"
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

constexpr int HEADER = 10;//SIZE OF HEADER
namespace Libraries{

    struct id3p_file_info{
        //u8 id3_format = 0;
        //u8 version;
        u32 frame_start_position;
        size_t tag_size;
        u8 major_version;
        u8 minor_version;
    };

    class ID3Reader
    {
        id3p_file_info file_info;

        int  read_buf_frame(void* file_buffer, const char* framename, std::string& output);

        std::string decode_ISO_8859_1(const u8* data_chunk, const size_t& data_chunk_size);
        std::string decode_UTF_16_BOM(const u8* data_chunk, const size_t& data_chunk_size);
        std::string decode_UTF_16_BS(const u8* data_chunk, const size_t& data_chunk_size);
        std::string decode_UTF_8(const u8* data_chunk, const size_t& data_chunk_size);
        std::string decode_ID3_data_frame(const u8& encode_type_byte, const u8* data_chunk, const size_t& data_chunk_size);
        std::string utf_16_to_utf_8(char16_t* data_chunk, const size_t& data_chunk_size);

    public:
        //id3p_file_info file_info;

        ID3Reader(void* file_buffer, const size_t& file_size);
        ~ID3Reader();
        int read_buf_trackName(void* file_buffer, std::string& output);
        int read_buf_artist(void* file_buffer, std::string& output);
        int read_buf_apic(void* file_buffer, std::string& output);
    };

    ID3Reader::ID3Reader(void* file_buffer, const size_t& file_size)
    {
        u8* ptr = (u8*)file_buffer;
        u8* buff;
        size_t index = 3;
        u32 tag_size = 0;

        file_info.major_version = *(ptr + index);
        index++;
        file_info.minor_version = *(ptr + index);
        index++;
        index++;//flags ignore
        buff = (u8*)(ptr + index);
        index+=4;

        file_info.frame_start_position = index;
        file_info.tag_size = ( (buff[0] & 0x7F) << 21 ) | ( (buff[1] & 0x7F) << 14 ) | ( (buff[2] & 0x7F) << 7 ) | ( (buff[3] & 0x7F) );
    }

    ID3Reader::~ID3Reader(){}

    int ID3Reader::read_buf_frame(void* file_buffer, const char* framename, std::string& output)
    {
        u8* ptr = (u8*)file_buffer + file_info.frame_start_position;
        u32 tag_end = HEADER + file_info.tag_size;
        u8* buff;
        u32 index = 0;
        while(index < tag_end)
        {
            if (index + HEADER > tag_end)
                break;

            char* frame_name = (char*)(ptr + index);
            index+=4;
            buff = (u8*)(ptr + index);
            index+=4;
            u32 frame_size;

            if(file_info.major_version == 3)
                frame_size = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
            else if(file_info.major_version == 4)
                frame_size = ((buff[0] & 0x7F) << 21) | ((buff[1] & 0x7F) << 14) | ((buff[2] & 0x7F) << 7) | (buff[3] & 0x7F);

            if(frame_size == 0)
                continue;

            //skip flags
            int16_t flags = (buff[4] << 8) | (buff[5]);
            index+=2;

            if(std::strncmp(frame_name, framename, 4) == 0)
            {
                u8* encode_type = (u8*)(ptr + index);
                index+=1;
                u8* data_chunk = (u8*)(ptr + index);
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
        u8* ptr = (u8*)file_buffer + file_info.frame_start_position;
        u32 tag_end = HEADER + file_info.tag_size;
        u8* buff;
        u32 index = 0;
        while(index < tag_end)
        {
            if (index + HEADER > tag_end)
                break;

            char* frame_name = (char*)(ptr + index);
            index+=4;
            buff = (u8*)(ptr + index);
            index+=4;
            u32 frame_size;

            if(file_info.major_version == 3)
                frame_size = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
            else if(file_info.major_version == 4)
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
                u32 data_start_index = index;
                u8 encoding = ptr[index];//encoding
                index++;

                while(index < tag_end && (char)ptr[index] != '\0')
                {
                    index++;//skip MIME
                }

                index++;//skip \0
                index++;//skip pic type
                if(encoding == 0x00 || encoding == 0x03)
                {
                    //u16 start = index;
                    while((char)ptr[index] != '\0')
                    {
                        index++;//skip description
                    }
                    index++;//skip \0
                }
                else if(encoding == 0x01 || encoding == 0x02){
                    //u16 start = index;
                    while( (u16)( (ptr[index] << 8) | (ptr[index+1]) ) != 0x0000)
                    {
                        index+=2;//skip description
                    }
                    index+=2;//skip 00 00
                }
                //raw bytes start
                u32 headers_len = index - data_start_index;
                u32 image_data_size = frame_size - headers_len;

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


    std::string ID3Reader::decode_ID3_data_frame(const u8& encode_type_byte, const u8* data_chunk, const size_t& data_chunk_size)
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

    std::string ID3Reader::decode_ISO_8859_1(const u8* data_chunk, const size_t& data_chunk_size)
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

    std::string ID3Reader::decode_UTF_16_BOM(const u8* data_chunk, const size_t& data_chunk_size)
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
            u16 word;
            if (big_endian)
                word = (data_chunk[i] << 8) | data_chunk[i + 1];
            else
                word = (data_chunk[i + 1] << 8) | data_chunk[i];

            if (word == 0x0000) break;
            row_bytes.push_back(static_cast<char16_t>(word));
        }
        return utf_16_to_utf_8(row_bytes.data(), row_bytes.size());
    }

    std::string ID3Reader::decode_UTF_16_BS(const u8* data_chunk, const size_t& data_chunk_size)
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

    std::string ID3Reader::decode_UTF_8(const u8* data_chunk, const size_t& data_chunk_size)
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
        u8 byte;
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
}
