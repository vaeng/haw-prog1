#pragma once

#include "Point.h"

struct Point3D : public Point {
    int z;
    Point3D(int x, int y, int z);
    void move(int deltaX, int deltaY, int deltaZ);
    virtual void printPosition() const;
};
