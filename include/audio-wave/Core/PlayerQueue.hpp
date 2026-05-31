//#ifndef PLAYERQUUE_HPP
//#define PLAYERQUEU_HPP
#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <random>
namespace Core{
    class PlayerQueue
    {
        struct Track
        {
            std::string track_name;
            std::filesystem::path path_to_track;
        };

        uint64_t current_track = 0;
        std::mt19937 gen;

        public:
            std::vector<Track> track_queue;

            PlayerQueue() : gen(std::random_device{}()){};
            void fillQueue(const std::string& entry = "music");
            void printQueue();
            void setCurrentTrack(const int& position);
            void nextTrack(const int& position);
            int setRandomTrack();
            uint64_t currentIndex();
            std::filesystem::path selectTrack();
        
    };
}
//#endif
