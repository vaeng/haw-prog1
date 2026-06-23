#pragma once

namespace engine {

struct Vector2 {
    float x{}, y{};

    friend auto operator+(Vector2 lhs, const Vector2 &rhs) -> Vector2;
    friend auto operator*(Vector2 lhs, const Vector2 &rhs) -> Vector2;

    friend auto operator-(Vector2 lhs, const Vector2 &rhs) -> Vector2;

    friend auto operator==(const Vector2 &lhs, const Vector2 &rhs) -> bool;

    friend auto operator!=(const Vector2 &lhs, const Vector2 &rhs) -> bool;
};
} // namespace engine
