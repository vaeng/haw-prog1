#include "Point.h"
#include "Point3D.h"
#include <SFML/Graphics.hpp>
#include <iostream>

auto findLargest(int a, int b, int c) -> auto {
    auto largestAB = (a > b) ? a : b;
    return (largestAB > c) ? largestAB : c;
}

auto assignment2() -> void {
    int a = 0;
    int b = 0;
    int c = 0;
    std::cout << "Enter three numbers!\n";
    std::cout << "a: ";
    std::cin >> a;
    std::cout << "b: ";
    std::cin >> b;
    std::cout << "c: ";
    std::cin >> c;

    auto largest = findLargest(a, b, c);
    std::cout << "The largest number is: " << largest << "\n";
}

auto assignment3() -> void {
    Point point{0, 0};
    std::cout << "Initial position: (" << point.x << ", " << point.y << ")\n";
    point.move(5, 3);
    std::cout << "After moving: (" << point.x << ", " << point.y << ")\n";
}

auto globalMove(Point* point, int dx, int dy) -> void { point->move(dx, dy); }

auto globalMoveByValue(Point point, int dx, int dy) -> void { point.move(dx, dy); }

auto globalMoveByReference(Point& point, int dx, int dy) -> void { point.move(dx, dy); }

auto assignment4() -> void {

    // Create a point object in main() and call move(x,y) with parameters from user input.
    Point pointA{0, 0};
    pointA.move(2, 3);
    std::cout << "Position of pointA: (" << pointA.x << ", " << pointA.y << ")\n";

    // Create a point object pointer in main() and use "new" to create a new point object.
    auto* pointB = new Point{0, 0};
    std::cout << "Initial position of pointB: (" << pointB->x << ", " << pointB->y << ")\n";

    // Call move(x,y) on this pointer. Don't forget to "delete" the pointer object again.
    pointB->move(4, 6);
    std::cout << "After moving pointB: (" << pointB->x << ", " << pointB->y << ")\n";
    delete pointB;

    globalMoveByValue(pointA, 1, 1);
    std::cout << "After globalMove by value, pointA: (" << pointA.x << ", " << pointA.y << ")\n";

    globalMoveByReference(pointA, 3, 4);
    std::cout << "After globalMove by reference, pointA: (" << pointA.x << ", " << pointA.y
              << ")\n";

    /*

        Create a second class "3DPoint" that inherits from "Point" and has an additional z variable.

        Add a function "void printPosition()" to both "Point" and "3DPoint". It should output the
    x,y or x,y,z values to the command line.

        Create a vector<Point> in main() and put several Point and 3DPoint objects in this vector.
    Loop through this vector and call "printPosition()" on each object.
    */

    auto point1 = Point{0, 0};
    auto* point2 = &point1;
    std::cout << "Initial position of point1: (" << point1.x << ", " << point1.y << ")\n";
    std::cout << "Initial position of point2: (" << point2->x << ", " << point2->y << ")\n";
    globalMove(point2, 2, 5);
    std::cout << "After moving point2, point1: (" << point1.x << ", " << point1.y << ")\n";
    std::cout << "After moving point2, point2: (" << point2->x << ", " << point2->y << ")\n";

    auto point3D1 = Point3D{0, 0, 0};
    auto point3D2 = Point3D{1, 2, 3};
    point3D1.printPosition();
    point3D2.printPosition();

    auto points = std::vector<Point*>{&point1, &point3D1, &point3D2};
    for (const auto* point : points) {
        point->printPosition();
    }
}

auto main() -> int {
    /*
    sf::RenderWindow window(sf::VideoMode({800U, 600U}), "SFML on Windows");

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.display();
    }
    */

    assignment4();
    return 0;
}
