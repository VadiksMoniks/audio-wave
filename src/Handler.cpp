#include "Handler.hpp"
#include "WAVFormat.hpp"
#include "MP3Format.hpp"
#include "AudioPlayer.hpp"

std::unique_ptr<IAudioDecoder> Handler::processTrack(const std::filesystem::path& filename, AudioPlayer &player, SDL_Config& config, TrackInfo& info)
{
    info.artist = "Unknown artist";
    info.name = filename.stem().string();
    //IAudioDecoder* format = nullptr;
    std::unique_ptr<IAudioDecoder> format;

    if(filename.extension() == ".wav")
    {
        format = std::make_unique<WAVFormat>(filename);
    }
    else if(filename.extension() == ".mp3")
    {
        format = std::make_unique<MP3Format>(filename);
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
