#pragma once

#include "engine/Vector2.h"
#include <numbers>

namespace engine {

struct Transform {
    Vector2 position{};
    Vector2 scale{.x = 1.F, .y = 1.F};
    /// Rotation in radians, counter-clockwise, relative to the parent transform
    float rotation{};

    void setRotationDegrees(float degrees) { rotation = degrees * (std::numbers::pi / 180.f); }

    [[nodiscard]] float getRotationDegrees() const { return rotation * (180.f / std::numbers::pi); }
};
} // namespace engine
