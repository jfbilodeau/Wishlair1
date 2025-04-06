//
// Created by jfbil on 2024-11-14.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/geometry/Point.hpp"

namespace nomad {

class Circle {
public:
    Circle() = default;
    Circle(NomadInteger x, NomadInteger y, NomadInteger radius);

    [[nodiscard]] NomadInteger getX() const;
    [[nodiscard]] NomadInteger getY() const;
    [[nodiscard]] NomadInteger getRadius() const;
    [[nodiscard]] const Point& getCenter() const;
    [[nodiscard]] NomadInteger getLeft() const;
    [[nodiscard]] NomadInteger getRight() const;
    [[nodiscard]] NomadInteger getTop() const;
    [[nodiscard]] NomadInteger getBottom() const;

    void setX(NomadInteger x);
    void setY(NomadInteger y);
    void setRadius(NomadInteger radius);

private:
    Point m_center;
    NomadInteger m_radius = 0;
};

} // namespace nomad

