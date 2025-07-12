#ifndef TIMER
#define TIMER

#include "common.cpp"

uint64_t TICK_COUNTER = 0;

struct Timer{
    uint64_t lastTime;
    uint64_t currentTime;
    uint64_t interval;
};

bool TimerTick(Timer* timer){
    timer->currentTime = SDL_GetTicks();
    if(timer->currentTime >= timer->lastTime + timer->interval){
        timer->lastTime = timer->currentTime;
        return true;
    }
    return false;
}


bool TickTimerTick(Timer* timer){
    timer->currentTime = TICK_COUNTER;
    if(timer->currentTime >= timer->lastTime + timer->interval){
        timer->lastTime = timer->currentTime;
        return true;
    }
    return false;
}

#endif