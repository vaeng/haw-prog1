#pragma once

class Vector2 {
  public:
    double x, y;

    friend auto operator+(Vector2 lhs, const Vector2 &rhs) -> Vector2 {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    friend auto operator-(Vector2 lhs, const Vector2 &rhs) -> Vector2 {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }

    friend auto operator==(const Vector2 &lhs, const Vector2 &rhs) -> bool {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend auto operator!=(const Vector2 &lhs, const Vector2 &rhs) -> bool { return !(lhs == rhs); }

  private:
};