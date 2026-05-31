//#ifndef PLAYERUI_HPP
//#define PLAYERUI_HPP
#pragma once
#include "Core/SDL_Config.hpp"
#include "Core/TrackInfo.hpp"
#include <filesystem>
#include "SoundSys/IAudioDecoder.hpp"
#include "SoundSys/AudioPlayer.hpp"
#include <memory>
namespace Core{
    class Handler
    {
        public:
            std::unique_ptr<SoundSys::IAudioDecoder> processTrack(const std::filesystem::path& filename, SoundSys::AudioPlayer &player, SDL_Config& config, TrackInfo& info);
    };
}
//#endif
