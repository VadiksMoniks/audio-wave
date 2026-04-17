#include "Handler.hpp"
#include "WAVFormat.hpp"
#include "MP3Format.hpp"
#include "AudioPlayer.hpp"

IAudioDecoder* Handler::processTrack(const std::filesystem::path& filename, AudioPlayer &player, SDL_Config& config, TrackInfo& info)
{
    info.artist = "Unknown artist";
    info.name = filename.stem().string();
    IAudioDecoder* format = nullptr;

    if(filename.extension() == ".wav")
    {
        format = new WAVFormat(filename);
    }
    else if(filename.extension() == ".mp3")
    {
        format = new MP3Format(filename);
    }
    /*else if(filename.extension() == ".flac")
    {
        //format = new FLACFormat(filename);
        throw std::runtime_error("This format is unsupported yet\n");
    }*/

    config = format->open(info);
    player.setDevice(config);
    player.prepareBuffer(config);

    return format;
}
