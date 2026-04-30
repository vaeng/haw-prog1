#include "Point3D.h"

#include <iostream>

Point3D::Point3D(int x, int y, int z) : Point(x, y), z{z} {}

void Point3D::move(int deltaX, int deltaY, int deltaZ) {
    Point::move(deltaX, deltaY);
    z += deltaZ;
}

void Point3D::printPosition() const {
    std::cout << "Point3D: (" << x << ", " << y << ", " << z << ")\n";
}
