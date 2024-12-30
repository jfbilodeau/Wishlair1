//
// Created by jfbil on 2024-11-14.
//

#include "nomad/geometry/CircleF.hpp"

namespace nomad {

CircleF::CircleF(NomadFloat x, NomadFloat y, NomadFloat radius):
    m_center(x, y),
    m_radius(radius)
{
}

NomadFloat CircleF::get_x() const {
    return m_center.x();
}

NomadFloat CircleF::get_y() const {
    return m_center.y();
}

NomadFloat CircleF::get_radius() const {
    return m_radius;
}

const PointF& CircleF::get_center() const {
    return m_center;
}

NomadFloat CircleF::get_left() const {
    return m_center.x() - m_radius;
}

NomadFloat CircleF::get_right() const {
    return m_center.x() + m_radius;
}

NomadFloat CircleF::get_top() const {
    return m_center.y() - m_radius;
}

NomadFloat CircleF::get_bottom() const {
    return m_center.y() + m_radius;
}

void CircleF::set_x(NomadFloat x) {
    m_center.set_x(x);
}

void CircleF::set_y(NomadFloat y) {
    m_center.set_y(y);
}

void CircleF::set_radius(NomadFloat radius) {
    m_radius = radius;
}

} // namespace nomad