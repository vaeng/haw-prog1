#include "game/GameObject.h"
#include "game/Component.h"

#include <algorithm>
#include <typeindex>

auto GameObject::getParent() -> GameObject * { return _parent; }

auto GameObject::isRoot() -> bool { return _parent == nullptr; }

auto GameObject::reparent(GameObject *newParent) -> void {
    if (_parent == newParent) {
        return;
    }

    if (_parent != nullptr) {
        auto &siblings = _parent->_children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    _parent = newParent;

    if (newParent != nullptr) {
        newParent->_children.push_back(this);
    }
}

auto GameObject::addChild(GameObject *child) -> void {
    if (child != nullptr) {
        child->reparent(this);
    }
}

auto GameObject::hasChildren() -> bool { return !_children.empty(); }

auto GameObject::getChildren() -> std::vector<GameObject *> & { return _children; }

auto GameObject::addComponent(Component *component) -> void {
    if (component != nullptr) {
        _components.push_back(component);
    }
}

auto GameObject::hasComponents() -> bool { return !_components.empty(); }

auto GameObject::getComponents() -> std::vector<Component *> & { return _components; }

auto GameObject::getComponentOfType(const std::type_index &type) -> Component * {
    for (auto *component : _components) {
        if (std::type_index(typeid(*component)) == type) {
            return component;
        }
    }
    return nullptr;
}

auto GameObject::getWorldTransform() -> Transform {
    if (isRoot()) {
        return transform;
    }
    auto parentTransform = getParent()->getWorldTransform();
    parentTransform.position = parentTransform.position + transform.position;
    parentTransform.scale = Vector2(parentTransform.scale.x * transform.scale.x,
                                    parentTransform.scale.y * transform.scale.y);
    return parentTransform;
}