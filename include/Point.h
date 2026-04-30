#pragma once

struct Point {
    int x;
    int y;
    Point(int x, int y);
    void move(int deltaX, int deltaY);
    void printPosition() const;
};
