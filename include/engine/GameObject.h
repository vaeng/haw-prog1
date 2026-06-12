#pragma once

#include <memory>
#include <vector>

#include "engine/Component.h"
#include "engine/Transform.h"

namespace engine {

class Core;

class GameObject {
  public:
    bool enabled{true};

    Transform localTransform{};
    [[nodiscard]] auto getWorldTransform() -> Transform;

    auto getParent() -> GameObject *;
    [[nodiscard]] auto isRoot() const -> bool;

    auto addChild(std::unique_ptr<GameObject> child) -> GameObject &;

    [[nodiscard]] auto hasChildren() const -> bool;
    auto getChildren() -> std::vector<std::unique_ptr<GameObject>> &;

    template <typename T, typename... Args> auto addComponent(Args &&...args) -> T & {
        static_assert(std::is_base_of_v<Component, T>);

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;
        T &ref = *component;

        _components.push_back(std::move(component));
        return ref;
    }

    [[nodiscard]] auto hasComponents() const -> bool;

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

    auto getCore() -> Core * {
        if (_core != nullptr) {
            return _core;
        }
        throw std::runtime_error("GameObject is not attached to a Core");
    }

    void setCore(Core *core) {
        _core = core;
        for (auto &child : _children) {
            child->setCore(core);
        }
    }

  private:
    Core *_core{nullptr};
    GameObject *_parent{nullptr}; // non-owning, set once in addChild, null for root
    std::vector<std::unique_ptr<GameObject>> _children;
    std::vector<std::unique_ptr<Component>> _components;
};
} // namespace engine
