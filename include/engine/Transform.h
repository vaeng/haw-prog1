#pragma once

#include "engine/Vector2.h"

namespace engine {

class Transform {
  public:
    Vector2 position{};
    Vector2 scale{.x = 1.F, .y = 1.F};
    float rotation{};

  private:
};
} // namespace engine
