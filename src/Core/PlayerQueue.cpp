#include "Core/PlayerQueue.hpp"
#include <fstream>
#include "Libraries/ID3Reader.hpp"
#include "Libraries/RIFFReader.hpp"
#include <algorithm>

namespace Core{
    void PlayerQueue::fillQueue(const std::string& entry)
    {
        std::vector<uint8_t> inner_buffer;
        std::ifstream file;
        std::string name = "";
        std::string artist = "";
        std::string apic = "";

        for(const auto& it : std::filesystem::directory_iterator(entry))
        {
            inner_buffer.resize(std::filesystem::file_size(it));
            file.open(it.path(), std::ios_base::binary);
            file.read((char*)inner_buffer.data(), inner_buffer.size());
            file.close();

            if(std::filesystem::is_regular_file(it) && std::filesystem::path(it).extension() == ".mp3"){

                Libraries::ID3Reader reader((void*)inner_buffer.data(), inner_buffer.size());
                if(reader.file_info.id3_format == 2)
                {
                    if(reader.read_buf_trackName((void*)inner_buffer.data(), name) == 0)
                        name = std::filesystem::path(it).stem().string();
                    if(reader.read_buf_artist((void*)inner_buffer.data(), artist) == 0)
                        artist = "Unknown";
                    if(reader.read_buf_apic((void*)inner_buffer.data(), apic) == 0)
                        apic = "";
                }
                else{
                    name = std::filesystem::path(it).stem().string();
                    artist = "Unknown";
                    apic = "";
                }
                this->fitString(name);
                this->fitString(artist);

                track_queue.push_back({name, artist, apic, std::filesystem::path(it)});
            }
            else if(std::filesystem::is_regular_file(it) && std::filesystem::path(it).extension() == ".wav")
            {
                Libraries::RIFFReader reader(inner_buffer.size());
                if(reader.read_buf_trackName((void*)inner_buffer.data(), name) == 0)
                    name = std::filesystem::path(it).stem().string();
                if(reader.read_buf_artist((void*)inner_buffer.data(), artist) == 0)
                    artist = "Unknown";

                apic = "";
                this->fitString(name);
                this->fitString(artist);

                track_queue.push_back({name, artist, apic, std::filesystem::path(it)});
            }
            else if(std::filesystem::is_directory(it))
            {
                fillQueue(entry + '/' + std::filesystem::path(it).filename().string());
            }
            inner_buffer.clear();
            file.clear();
        }

        /*sort(track_queue.begin(), track_queue.end(), [](const Track& t1, const Track& t2){
            return t1.track_name < t2.track_name;
        });*/
    }

    uint64_t PlayerQueue::currentIndex()
    {
        return current_track;
    }

    void PlayerQueue::setCurrentTrack(const int& position)
    {
        if(position >= 0 && position < track_queue.size())
        {
            current_track = position;
        }
        else{//?????
            current_track = 0;
        }
    }

    void PlayerQueue::nextTrack(const int& position)
    {

        if(position < 0)
        {
            if(current_track == 0)
            {
                current_track = track_queue.size() - 1;
            }
            else{
                current_track --;
            }
        }
        else{
            current_track += position;

            if(current_track > track_queue.size() - 1)
            {
                current_track = 0;
            }
        }
    }

    int PlayerQueue::setRandomTrack()
    {
        std::uniform_int_distribution<int> dist(0, track_queue.size()-1);
        return dist(gen);
    }

    std::filesystem::path PlayerQueue::selectTrack()
    {
        return track_queue[current_track].path_to_track;
    }

    void PlayerQueue::fitString(std::string& string)
    {
        int size = 0;
        for(int i = 0; i < string.length(); ++i)
        {
            unsigned char c = (unsigned char)string[i];

            if(size == 25)
            {
                string.resize(i);
                string += "...";
                return;
            }

            if((c >> 4) == 0xF)
                i+=3;
            else if((c >> 4) == 0xE)
                i+=2;
            else if((c >> 4) == 0xD || (c >> 4) == 0xC)
                i+=1;

            size ++;
        }
    }

    std::string PlayerQueue::getCurrentTrackName()
    {
        return track_queue[this->current_track].track_name;
    }

    std::string PlayerQueue::getCurrentTrackArtist()
    {
        return track_queue[this->current_track].track_artist;
    }

    std::string PlayerQueue::getCurrentTrackApic()
    {
        return track_queue[this->current_track].apic;
    }
}
