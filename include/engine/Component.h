#pragma once

#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>

namespace engine {

class GameObject;

class Component {
  public:
    virtual ~Component() = default;

    virtual void update(float deltaTime) {}
    virtual void handleEvent(const std::optional<sf::Event> &event, float deltaTime) {}
    virtual void start() {}
    virtual auto clone() const -> std::unique_ptr<Component> = 0;
    // virtual void serialize(nlohmann::json &j) const = 0;
    bool enabled{true};
    GameObject *owner{nullptr};
};
} // namespace engine
