#ifndef EVENT_SYSTEM
#define EVENT_SYSTEM

#include "common.cpp"

enum class EventType {
    None,
    BlockBreak,
    BlockPlaced,
    PacketClientSend,
    PacketServerSend,
    ClientEntityCreate, //TODO FIX THESE DAMN NAMES
    HandshakeEntityCreate, //TODO FIX THERE DAMN NAMES
    EntityAction,
};

struct Event {
    EventType type;
    void* data = nullptr;  // Optional payload
};

//kinda stupid to do this on the framework...
struct BlockBreakEvent {
    glm::ivec3 position;
};

struct BlockPlacedEvent{
    glm::ivec3 position;
    uint32_t id;
    uint32_t flags;
};

struct PacketClientSendEvent{
    uint8_t* data;
    size_t size;
    uint8_t packetType;
};

struct PacketServerSendEvent{
    uint8_t* data;
    size_t size;
    size_t size2; //For extra data if neccessary??
    uint32_t clientID;
    uint8_t packetType;
};

struct ClientEntityCreateEvent{
    uint32_t clientID;
};

struct HandshakeEntityCreateEvent{
    uint8_t* data;
    uint32_t entityCount;
};

struct EntityActionEvent{
    uint32_t clientID;
    uint32_t actionCode;
    glm::ivec3 position;
};

struct EventSystem {
    using Listener = std::function<void(const Event&)>;

    static EventSystem& Instance() {
        static EventSystem instance = {};         
        return instance;
    }

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