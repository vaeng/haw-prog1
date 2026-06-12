#include "engine/GameObject.h"
#include "engine/Component.h"

namespace engine {

auto GameObject::getParent() -> GameObject * { return _parent; }

auto GameObject::isRoot() const -> bool { return _parent == nullptr; }

auto GameObject::addChild(std::unique_ptr<GameObject> child) -> GameObject & {
    assert(child->_parent == nullptr && "Child already has a parent");
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

auto GameObject::getWorldTransform() -> Transform {
    if (isRoot()) {
        return localTransform;
    }
    auto worldTransform = getParent()->getWorldTransform();
    worldTransform.position = worldTransform.position + localTransform.position;
    worldTransform.scale = Vector2(worldTransform.scale.x * localTransform.scale.x,
                                   worldTransform.scale.y * localTransform.scale.y);
    worldTransform.rotation += localTransform.rotation;
    return worldTransform;
}
} // namespace engine
