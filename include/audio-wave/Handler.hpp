#ifndef PLAYERUI_HPP
#define PLAYERUI_HPP
#include "SDL_Config.hpp"
#include "TrackInfo.hpp"
#include <filesystem>
#include "IAudioDecoder.hpp"
#include "AudioPlayer.hpp"

class Handler
{
    public:
        IAudioDecoder* processTrack(const std::filesystem::path& filename, AudioPlayer &player, SDL_Config& config, TrackInfo& info);
};
#endif
