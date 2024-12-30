//
// Created by jfbil on 2024-11-14.
//

#include "nomad/geometry/Circle.hpp"

namespace nomad {

Circle::Circle(NomadInteger x, NomadInteger y, NomadInteger radius):
    m_center(x, y),
    m_radius(radius)
{
}

NomadInteger Circle::get_x() const {
    return m_center.x();
}

NomadInteger Circle::get_y() const {
    return m_center.y();
}

NomadInteger Circle::get_radius() const {
    return m_radius;
}

const Point& Circle::get_center() const {
    return m_center;
}

NomadInteger Circle::get_left() const {
    return m_center.x() - m_radius;
}

NomadInteger Circle::get_right() const {
    return m_center.x() + m_radius;
}

NomadInteger Circle::get_top() const {
    return m_center.y() - m_radius;
}

NomadInteger Circle::get_bottom() const {
    return m_center.y() + m_radius;
}

void Circle::set_x(NomadInteger x) {
    m_center.set_x(x);
}

void Circle::set_y(NomadInteger y) {
    m_center.set_y(y);
}

void Circle::set_radius(NomadInteger radius) {
    m_radius = radius;
}

} // namespace nomad