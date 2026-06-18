#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace engine {

using HandlerID = size_t;

struct Subscriber {
    HandlerID id;
    std::function<void(const void *)> func;
};

class MessageBus;

struct Connection {
    HandlerID id;
    MessageBus *bus;
    ~Connection();
};

class MessageBus {
  public:
    template <typename MessageType> using HandlerFunc = std::function<void(const MessageType &)>;
    template <typename MessageType> Connection subscribe(HandlerFunc<MessageType> handler) {
        _nextHandlerID++;
        _handlers[typeid(MessageType)].push_back(
            Subscriber{.id = _nextHandlerID, .func = [handler](const void *msg) {
                           handler(*static_cast<const MessageType *>(msg));
                       }});
        return Connection{.id = _nextHandlerID, .bus = this};
    }
    template <typename MessageType> void publish(const MessageType &message) {
        auto it = _handlers.find(std::type_index(typeid(MessageType)));
        if (it == _handlers.end())
            return;
        for (auto &subscriber : it->second) {
            subscriber.func(&message);
        }
    }
    void unsubscribe(HandlerID id);

  private:
    HandlerID _nextHandlerID = 0;
    std::unordered_map<std::type_index, std::vector<Subscriber>> _handlers;
};

} // namespace engine