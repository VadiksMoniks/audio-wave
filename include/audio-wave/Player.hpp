#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <thread>
#include <atomic>
#include <mutex>
#include "Handler.hpp"
#include "PlayerQueue.hpp"
#include "PlayerState.hpp"
#include "AudioPlayer.hpp"
#include "IAudioDecoder.hpp"

enum class MODE
{
    LIST    = 0,
    REPEAT  = 1,
    SHUFFLE = 2,
};

class Player : public QObject
{
    Q_OBJECT;

signals:
    void currentTime(const std::string& time);
    void trackEnded();
    void currentProgress(uint32_t currentBytes, uint32_t totalBytes);

public:
    MODE play_mode = MODE::LIST;//BETTER HIDE INTO PRIVATE SECTION

    Player();
    ~Player();
    void audioLoop();
    void setTrack(const int &position);
    void repeat();
    void play();
    void pause();
    void get_slider_position(const int &value);
    void set_mode(const MODE& mode);
    PlayerState get_current_state();
    void set_current_state(const PlayerState& state);
    std::string count_total_time();
    std::string count_current_time();
    std::string get_track_name();
    std::string get_track_artist();
    std::string get_apic();
    //ПЕРЕДЕЛАТЬ
    PlayerQueue q;

private:
    IAudioDecoder* decoder = nullptr;
    std::atomic<PlayerState>CURRENT_STATE = PlayerState::STOPPED;
    SDL_Config config;
    TrackInfo info;
    Handler h;
    AudioPlayer player;
    std::thread audioThread;
    std::atomic<bool> isRunning{true};
    std::mutex mx;
    //MODE play_mode = MODE::LIST;
};

#endif // PLAYER_H
