#include "engine/MessageBus.h"

namespace engine {
// Move constructor
Connection::Connection(Connection &&other) noexcept : id(other.id), bus(other.bus) {
    other.bus = nullptr; // prevent double-unsubscribe
}
// Move assignment
Connection &Connection::operator=(Connection &&other) noexcept {
    if (this != &other) {
        if (bus)
            bus->unsubscribe(id);
        id = other.id;
        bus = other.bus;
        other.bus = nullptr;
    }
    return *this;
}

Connection::~Connection() {
    if (bus) {
        bus->unsubscribe(id);
    }
}

void MessageBus::unsubscribe(HandlerID id) {
    for (auto &[type, subscribers] : _handlers) {
        std::erase_if(subscribers, [id](const Subscriber &s) { return s.id == id; });
    }
}
} // namespace engine