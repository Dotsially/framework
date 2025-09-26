#ifndef TIMER
#define TIMER

#include "common.cpp"

uint64_t TICK_COUNTER = 0;
std::chrono::steady_clock::time_point EngineStartTime;

void EngineStartClock(){
    EngineStartTime = std::chrono::steady_clock::now();
}

//Returns time in ms since engine start
uint64_t EngineGetTime() {
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - EngineStartTime).count();
    return uint64_t(ms);
}


class Timer{
public:
    uint64_t m_lastTime;
    uint64_t m_currentTime;
    uint64_t m_interval;

    
    bool Tick(){
        m_currentTime = EngineGetTime();
        if(m_currentTime >= m_lastTime + m_interval){
            m_lastTime = m_currentTime;
            return true;
        }
        return false;
    }

    //Uses TICK_COUNTER instead of EngineStartTime
    bool CounterTick(){
        m_currentTime = TICK_COUNTER;
        if(m_currentTime >= m_lastTime + m_interval){
            m_lastTime = m_currentTime;
            return true;
        }
        return false;
    }
};

#endif