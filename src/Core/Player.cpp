#include "Core/Player.hpp"
namespace Core{
    Player::Player()
    {
        audioThread = std::thread(&Player::audioLoop, this);
        audioThread.detach();
    }

    Player::~Player()
    {
        isRunning = false;
        //delete decoder;
        //decoder = nullptr;
        decoder.reset();
    }

    void Player::set_mode(const MODE& mode)
    {
        if(play_mode == mode)
        {
            play_mode = MODE::LIST;
        }
        else{
        play_mode = mode;  
        }
    }

    MODE Player::get_current_mode()
    {
        return play_mode;
    }

    void Player::set_volume(const int& volume)
    {
        player.setVolume((float)(volume/100.0));
    }

    PlayerState Player::get_current_state()
    {
        return CURRENT_STATE;
    }

    void Player::set_current_state(const PlayerState& state)
    {
        CURRENT_STATE = state;
    }

    void Player::play()
    {
        CURRENT_STATE = PlayerState::PLAYING;
        player.play();
    }

    void Player::pause()
    {
        CURRENT_STATE = PlayerState::PAUSED;
        player.pause();
    }

    void Player::setTrack(const int &position)
    {
        pause();
        std::lock_guard<std::mutex> lock(mx);

        q.nextTrack(position);
        std::filesystem::path track = q.selectTrack();

        //delete decoder;
        //decoder = nullptr;
        decoder.reset();
        decoder = std::move(h.processTrack(track, player, config));
    }

    void Player::repeat()
    {
        if(decoder)
        {
        decoder->repeat(); 
        }  
        else{
            setTrack(0);
        }
    }

    /*std::string Player::get_apic()
    {
        return info.apic;
    }*/

    void Player::get_slider_position(const int& value)
    {
        pause();

        std::lock_guard<std::mutex> lock(mx);
        if(decoder)
        {
            uint32_t position = value - value % 4;
            decoder->setPosition(position);
        }

    }

    std::string Player::count_total_time()
    {
        if(decoder)
        {
            double total_time = ( decoder->getChunkSize() / ( config.numChannels * (config.bitsPerSample/8.0) * config.sampleRate ) );
            uint32_t total_min = total_time / 60;
            uint32_t total_sec = (uint32_t)total_time % 60;

            std::string time = std::to_string(total_min) + " : ";

            if(total_sec < 10)
            {
                time +="0"+std::to_string(total_sec);
            }
            else{
                time +=std::to_string(total_sec);
            }
            return time;
        }
        else{
            return "0:00";
        }
    }

    std::string Player::count_current_time()
    {
        if(decoder)
        {
            uint32_t played_min;
            uint32_t played_sec;
            uint32_t bytes_out_of_device;
            double current_time_total;

            bytes_out_of_device = ( decoder->getCurrentPosition() - player.getSDLQueuedAudio() );
            current_time_total = bytes_out_of_device / config.byte_rate;

            played_min = (uint32_t)current_time_total / 60;
            played_sec = (uint32_t)current_time_total % 60;

            std::string time = std::to_string(played_min) + " : ";

            if(played_sec < 10)
            {
                time +="0"+std::to_string(played_sec);
            }
            else{
                time +=std::to_string(played_sec);
            }
            return time;
        }
        else{
            return "0:00";
        }
    }

    void Player::audioLoop() {
        int progress;

        while(isRunning) {
            if(CURRENT_STATE == PlayerState::PLAYING) {
                std::lock_guard<std::mutex> lock(mx);
                if(!player.playChunk(decoder.get())) {
                    emit trackEnded();
                }
            }

            emit currentTime(count_current_time());

            if(decoder)
            {
                emit currentProgress(decoder->getCurrentPosition(), decoder->getChunkSize());
            }
            SDL_Delay(20);
        }
    }
}