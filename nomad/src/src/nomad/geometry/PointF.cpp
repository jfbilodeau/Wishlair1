//
// Created by jfbil on 2023-06-07.
//

#include "nomad/geometry/PointF.hpp"

#include <cmath>

namespace nomad {

PointF::PointF():
    m_x(0),
    m_y(0) {
}

PointF::PointF(Coord x, Coord y):
    m_x(x),
    m_y(y)
{
}

NomadFloat PointF::distance_to(const PointF& other) const {
    return std::sqrt(
        std::pow(m_x - other.m_x, 2) +
        std::pow(m_y - other.m_y, 2)
    );
}

NomadFloat PointF::distance_to(Coord x, Coord y) const {
    return std::sqrt(
        std::pow(m_x - x, 2) +
        std::pow(m_y - y, 2)
    );
}

SDL_Point& PointF::to_sdl_point(SDL_Point& point) const {
    point.x = int(m_x);
    point.y = int(m_y);

    return point;
}

SDL_Point PointF::to_sdl_point() const {
    return SDL_Point{int(m_x), int(m_y)};
}

SDL_FPoint PointF::to_sdl_fpoint() const {
    return SDL_FPoint{float(m_x), float(m_y)};
}

SDL_FPoint& PointF::to_sdl_fpoint(SDL_FPoint& point) const {
    point.x = float(m_x);
    point.y = float(m_y);

    return point;
}

} // nomad