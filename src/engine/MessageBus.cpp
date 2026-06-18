#include "engine/MessageBus.h"

namespace engine {
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