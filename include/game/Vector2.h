#pragma once

class Vector2 {
  public:
    double x, y;

    friend auto operator+(Vector2 lhs, const Vector2 &rhs) -> Vector2;

    friend auto operator-(Vector2 lhs, const Vector2 &rhs) -> Vector2;

    friend auto operator==(const Vector2 &lhs, const Vector2 &rhs) -> bool;

    friend auto operator!=(const Vector2 &lhs, const Vector2 &rhs) -> bool;

  private:
};