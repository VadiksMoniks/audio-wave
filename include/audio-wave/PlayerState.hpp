#ifndef PLAYERSTATE_HPP
#define PLAYERSTATE_HPP

enum class PlayerState : int{
    PLAYING = 0,
    PAUSED  = 1,
    STOPPED = 2,
    ENDED   = 3,
    EXIT    = 4,
};

#endif