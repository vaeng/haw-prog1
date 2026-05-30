#pragma once

#include <typeindex>
#include <vector>

class Component;

#include "game/Transform.h"

class GameObject {
  public:
    bool enabled{true};

    Transform localTransform{};
    auto getWorldTransform() -> Transform;

    auto getParent() -> GameObject *;
    auto isRoot() -> bool;
    auto reparent(GameObject *newParent) -> void;

    auto addChild(GameObject *child) -> void;
    auto hasChildren() -> bool;
    auto getChildren() -> std::vector<GameObject *> &;

    auto addComponent(Component *component) -> void;
    auto hasComponents() -> bool;
    auto getComponents() -> std::vector<Component *> &;
    auto getComponentOfType(const std::type_index &type) -> Component *;

  private:
    GameObject *_parent{nullptr};
    std::vector<GameObject *> _children;
    std::vector<Component *> _components;
};