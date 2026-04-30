#include "Point.h"

#include <iostream>

Point::Point(int x, int y) : x{x}, y{y} {}

void Point::move(int deltaX, int deltaY) {
    x += deltaX;
    y += deltaY;
}

void Point::printPosition() const { std::cout << "Point: (" << x << ", " << y << ")\n"; }