#include "Core/Handler.hpp"
#include "SoundSys/WAVFormat.hpp"
#include "SoundSys/MP3Format.hpp"
#include "SoundSys/AudioPlayer.hpp"
namespace Core{
    std::unique_ptr<SoundSys::IAudioDecoder> Handler::processTrack(const std::filesystem::path& filename, SoundSys::AudioPlayer &player, SDL_Config& config, TrackInfo& info)
    {
        info.artist = "Unknown artist";
        info.name = filename.stem().string();
        //IAudioDecoder* format = nullptr;
        std::unique_ptr<SoundSys::IAudioDecoder> format;

        if(filename.extension() == ".wav")
        {
            format = std::make_unique<SoundSys::WAVFormat>(filename);
        }
        else if(filename.extension() == ".mp3")
        {
            format = std::make_unique<SoundSys::MP3Format>(filename);
        }
        /*else if(filename.extension() == ".flac")
        {
            //format = std::make_unique<FLACFormat>(filename);
            throw std::runtime_error("This format is unsupported yet\n");
        }*/

        config = format->open(info);
        player.setDevice(config);

        return format;
    }
}