#ifndef EVENT_SYSTEM
#define EVENT_SYSTEM

#include "common.cpp"

enum class EventType {
    None,
    PacketReceived
};

struct Event {
    EventType type;
    void* data = nullptr;  // Optional payload
};

struct PacketReceivedEvent {
    uint8_t* data;
    size_t size;
};

struct EventSystem {
    using Listener = std::function<void(const Event&)>;
    std::unordered_map<EventType, std::vector<Listener>> listeners;
    
    void Subscribe(EventType type, Listener listener) {
        listeners[type].push_back(listener);
    }

    void Fire(EventType type, void* data = nullptr) {
        Event event{type, data};
        for (const auto& listener : listeners[type]) {
            listener(event);
        }
    }
};


#endif