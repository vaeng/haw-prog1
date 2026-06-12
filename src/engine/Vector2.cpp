#include "engine/Vector2.h"

namespace engine {

auto operator+(Vector2 lhs, const Vector2 &rhs) -> Vector2 {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

auto operator-(Vector2 lhs, const Vector2 &rhs) -> Vector2 {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

auto operator==(const Vector2 &lhs, const Vector2 &rhs) -> bool {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

auto operator!=(const Vector2 &lhs, const Vector2 &rhs) -> bool { return !(lhs == rhs); }

} // namespace engine
