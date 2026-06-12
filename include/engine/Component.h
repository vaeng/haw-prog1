#pragma once

#include <SFML/Window/Event.hpp>
#include <optional>

namespace engine {

class GameObject;

class Component {
  public:
    virtual ~Component() = default;

    virtual void update(float deltaTime) {}
    virtual void handleEvent(const std::optional<sf::Event> &event, float deltaTime) {}
    virtual void start() {}

    bool enabled{true};
    GameObject *owner{nullptr};
};
} // namespace engine
