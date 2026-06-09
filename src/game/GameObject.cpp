#include "game/GameObject.h"
#include "game/Component.h"

auto GameObject::getParent() -> GameObject * { return _parent; }

auto GameObject::isRoot() -> bool { return _parent == nullptr; }

auto GameObject::addChild(std::unique_ptr<GameObject> child) -> GameObject & {
    child->_parent = this;
    _children.push_back(std::move(child));
    return *_children.back();
}

auto GameObject::hasChildren() -> bool { return !_children.empty(); }

auto GameObject::getChildren() -> std::vector<std::unique_ptr<GameObject>> & { return _children; }

auto GameObject::hasComponents() -> bool { return !_components.empty(); }

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