#pragma once

#include "Vector2.h"

class Transform {
  public:
    Vector2 position{};
    Vector2 scale{.x = 1.0, .y = 1.0};
    double rotation{0.0};

  private:
};