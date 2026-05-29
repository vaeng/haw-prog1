#pragma once

#include <Vector>

#include "Transform.h"

class GameObject {
  public:
    Transform transform{};

  private:
    GameObject *_parent{nullptr};
    std::vector<GameObject *> _children;
};