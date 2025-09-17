#ifndef TIMER
#define TIMER

#include "common.cpp"

uint64_t TICK_COUNTER = 0;
std::chrono::steady_clock::time_point AppStartTime;

struct Timer{
    uint64_t lastTime;
    uint64_t currentTime;
    uint64_t interval;
};

uint64_t GetMillisecondsSinceStart() {
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - AppStartTime).count();
    return uint64_t(ms);
}

bool TimerTick(Timer* timer){
    timer->currentTime = GetMillisecondsSinceStart();
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