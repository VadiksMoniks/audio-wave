//#ifndef PLAYERUI_HPP
//#define PLAYERUI_HPP
#pragma once
#include "SDL_Config.hpp"
#include "TrackInfo.hpp"
#include <filesystem>
#include "IAudioDecoder.hpp"
#include "AudioPlayer.hpp"
#include <memory>

class Handler
{
    public:
        std::unique_ptr<IAudioDecoder> processTrack(const std::filesystem::path& filename, AudioPlayer &player, SDL_Config& config, TrackInfo& info);
};
//#endif
