#pragma once

#include <memory>
#include <vector>

#include "engine/Component.h"
#include "engine/Transform.h"

namespace engine {

class Core;

/// A GameObject represents an entity in the game world
///
/// It has a transform and a parent, an enabled flag, and can have components, and child GameObjects
class GameObject {
  public:
    /// The enabled flag determines whether the GameObject and its components are active
    ///
    /// Determines if they should be updated and rendered. Disabling a GameObject will also disable
    /// all of its children, but not its parent.
    bool enabled{true};

    /// The local transform of the GameObject, relative to its parent
    Transform localTransform{};

    /// The world transform of the GameObject
    ///
    /// This is the local transform combined with the transforms of all its ancestors. This is what
    /// should be used for rendering and physics calculations,
    [[nodiscard]] auto getWorldTransform() -> Transform;

    /// The parent GameObject, or null if this is the root or detached
    auto getParent() -> GameObject *;

    /// Checks if this GameObject is the root of the scene (i.e. has no parent)
    [[nodiscard]] auto isRoot() const -> bool;

    /// Adds a child GameObject to this GameObject
    ///
    /// The child will be owned by this GameObject, and its parent will be set to this GameObject.
    /// Returns a reference to the added child for convenience. Throws an exception if the child is
    /// already owned by another GameObject.
    auto addChild(std::unique_ptr<GameObject> child) -> GameObject &;

    [[nodiscard]] auto hasChildren() const -> bool;

    /// Gets the children of this GameObject
    ///
    /// Returns a reference to the vector of unique pointers to the child GameObjects. Modifying
    /// this vector will modify the children of this GameObject, but be careful not to break the
    /// parent-child relationship (e.g. by moving a child to another GameObject without updating its
    /// parent pointer).
    auto getChildren() -> std::vector<std::unique_ptr<GameObject>> &;

    /// Adds a component of type T to this GameObject
    ///
    /// The component will be owned by this GameObject, and its owner pointer will be set to this
    /// GameObject. Returns a reference to the added component for convenience.
    template <typename T, typename... Args> auto addComponent(Args &&...args) -> T * {
        static_assert(std::is_base_of_v<Component, T>);

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;
        T *ptr = component.get();

        _components.push_back(std::move(component));
        return ptr;
    }

    [[nodiscard]] auto hasComponents() const -> bool;

    /// Gets a component of type T of this GameObject
    ///
    /// Returns a pointer to the component if it exists, or null if it doesn't.
    /// If there are multiple components of type T, returns the first one found. This is not
    /// recommended and may be changed in the future to return a vector of components of type T
    /// instead
    template <typename T> auto getComponent() -> T * {
        for (auto &component : _components) {
            if (auto casted = dynamic_cast<T *>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    /// Starts all components of this GameObject
    ///
    /// This should be called once, after the GameObject is added to a scene and before the game
    /// loop starts.
    auto startComponents() {
        for (auto &component : _components) {
            if (component->enabled) {
                component->start();
            }
        }
    }

    /// Updates all components of this GameObject
    ///
    /// This should be called every frame, typically from the game loop.
    auto updateComponents(float deltaTime) {
        for (auto &component : _components) {
            if (component->enabled) {
                component->update(deltaTime);
            }
        }
    }

    /// Handles an event for all components of this GameObject
    ///
    /// This should be called every frame for every event since the last frame, typically called
    /// from the game loop
    auto handleEventComponents(const std::optional<sf::Event> &event, float deltaTime) {
        for (auto &component : _components) {
            if (component->enabled) {
                component->handleEvent(event, deltaTime);
            }
        }
    }

    /// Gets the Core that this GameObject is attached to
    ///
    /// This is used to access the window and other global resources. Throws an exception if this
    /// GameObject is not attached to a Core (i.e. is not part of a scene).
    auto getCore() -> Core * {
        if (_core != nullptr) {
            return _core;
        }
        throw std::runtime_error("GameObject is not attached to a Core");
    }

    /// Sets the Core that this GameObject is attached to
    void setCore(Core *core) {
        _core = core;
        for (auto &child : _children) {
            child->setCore(core);
        }
    }

    /// Deep-clones this GameObject, its components, and its children.
    ///
    /// The clone is detached — it has no parent, no Core, and components have not
    /// been started. Call setCore() then startComponents() before use.
    auto clone() -> std::unique_ptr<GameObject> {
        auto cloned = std::make_unique<GameObject>();
        cloned->enabled = enabled;
        cloned->localTransform = localTransform;
        for (const auto &component : _components) {
            // This requires that all components have a copy constructor, which is a reasonable
            // requirement. If a component has pointers or other non-trivial state, it should handle
            // copying that state in its copy constructor.
            cloned->_components.push_back(component->clone());
            cloned->_components.back()->owner = cloned.get();
        }
        for (const auto &child : _children) {
            cloned->addChild(child->clone());
        }
        return cloned;
    }

  private:
    Core *_core{nullptr};
    GameObject *_parent{
        nullptr}; // non-owning, set once in addChild, null for root or detached GameObjects
    std::vector<std::unique_ptr<GameObject>> _children;
    std::vector<std::unique_ptr<Component>> _components;
};
} // namespace engine
