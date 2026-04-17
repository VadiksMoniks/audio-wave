#include "PlayerQueue.hpp"

void PlayerQueue::fillQueue(const std::string& entry)//ЕСЛИ МОЖНО БУДЕТ ВЫБИРАТЬ ПАПКУ - ТО НЕПОНЯТНО, ЧТО БУДЕТ С ОЧЕРЕДЬЮ,
{                                                    //ЛИБО НУЖЕН МЕТОД ДЛЯ ДОБАВЛЕНИЯ ФАЙЛОВ, ЛИБО ЕЩЕ ЧТО-ТО
    Track t;
    for(const auto& it : std::filesystem::directory_iterator(entry))
    {
        if(std::filesystem::is_regular_file(it) && ( std::filesystem::path(it).extension() == ".wav" || std::filesystem::path(it).extension() == ".mp3") )// || std::filesystem::path(it).extension() == ".flac") )
        {
            std::string name = std::filesystem::path(it).stem().string();
            int size = 0;
            for(int i = 0; i< name.length(); i++)
            {
                if(size == 25)
                {
                    name.resize(i);
                    name += "...";
                    break;
                }

                unsigned char c = (unsigned char)name[i];
                if((c >> 4) == 0xF)
                    i+=3;
                else if((c >> 4) == 0xE)
                    i+=2;
                else if((c >> 4) == 0xD || (c >> 4) == 0xC)
                    i+=1;

                size ++;
            }

            t = {name, std::filesystem::path(it)};
            track_queue.push_back(t);
        }
        else if(std::filesystem::is_directory(it))
        {
            fillQueue(entry + std::filesystem::path(it).filename().string());
        }
    }
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
