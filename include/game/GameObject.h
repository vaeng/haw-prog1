#pragma once

#include <memory>
#include <vector>

#include "game/Component.h"
#include "game/Transform.h"

#include <iostream>

class GameObject {
  public:
    bool enabled{true};

    Transform localTransform{};
    auto getWorldTransform() -> Transform;

    auto getParent() -> GameObject *;
    auto isRoot() -> bool;

    auto addChild(std::unique_ptr<GameObject> child) -> GameObject &;

    auto hasChildren() -> bool;
    auto getChildren() -> std::vector<std::unique_ptr<GameObject>> &;

    template <typename T, typename... Args> auto addComponent(Args &&...args) -> T & {
        static_assert(std::is_base_of_v<Component, T>);

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;
        T &ref = *component;

        _components.push_back(std::move(component));
        return ref;
    }

    auto hasComponents() -> bool;

    template <typename T> auto getComponent() -> T * {
        for (auto &component : _components) {
            if (auto casted = dynamic_cast<T *>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    auto startComponents() {
        for (auto &component : _components) {
            if (component->enabled) {
                component->start();
            }
        }
    }

    auto updateComponents(float deltaTime) {
        for (auto &component : _components) {
            if (component->enabled) {
                component->update(deltaTime);
            }
        }
    }

    auto handleEventComponents(const std::optional<sf::Event> &event, float deltaTime) {
        for (auto &component : _components) {
            if (component->enabled) {
                component->handleEvent(event, deltaTime);
            }
        }
    }

  private:
    GameObject *_parent{nullptr};
    std::vector<std::unique_ptr<GameObject>> _children;
    std::vector<std::unique_ptr<Component>> _components;
};
