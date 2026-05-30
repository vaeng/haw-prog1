#pragma once

class GameObject;

class Component {
  public:
    Component(GameObject *owner) : owner(owner) {}
    virtual ~Component() = default;

    bool enabled{true};
    GameObject *owner{nullptr};
};