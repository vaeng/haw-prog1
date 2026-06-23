#include "engine/GameObject.h"
#include "engine/Component.h"
#include <cmath>

namespace engine {

auto GameObject::getParent() const -> GameObject * { return _parent; }

auto GameObject::isRoot() const -> bool { return _parent == nullptr; }

auto GameObject::addChild(std::unique_ptr<GameObject> child) -> GameObject & {
    if (child->_parent != nullptr) {
        throw std::runtime_error("Child GameObject is already owned by another GameObject");
    }
    child->_parent = this;
    if (_core != nullptr) {
        child->setCore(_core);
    }
    _children.push_back(std::move(child));
    return *_children.back();
}

auto GameObject::hasChildren() const -> bool { return !_children.empty(); }

auto GameObject::getChildren() -> std::vector<std::unique_ptr<GameObject>> & { return _children; }

auto GameObject::hasComponents() const -> bool { return !_components.empty(); }

auto GameObject::getWorldTransform() const -> Transform {
    if (isRoot()) {
        return localTransform;
    }
    auto parentWorld = getParent()->getWorldTransform();
    Transform worldTransform{};
    worldTransform.rotation = parentWorld.rotation + localTransform.rotation;
    worldTransform.scale = parentWorld.scale * localTransform.scale;

    float scaledX = localTransform.position.x * parentWorld.scale.x;
    float scaledY = localTransform.position.y * parentWorld.scale.y;

    float cosA = std::cos(parentWorld.rotation);
    float sinA = std::sin(parentWorld.rotation);

    float rotatedX = cosA * scaledX - sinA * scaledY;
    float rotatedY = sinA * scaledX + cosA * scaledY;

    worldTransform.position =
        Vector2(parentWorld.position.x + rotatedX, parentWorld.position.y + rotatedY);

    return worldTransform;
}
} // namespace engine
