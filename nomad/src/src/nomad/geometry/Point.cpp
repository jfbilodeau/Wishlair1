//
// Created by jfbil on 2023-06-07.
//

#include "nomad/geometry/Point.hpp"

#include "nomad/geometry/PointF.hpp"

namespace nomad {

Point::Point():
    m_x(0),
    m_y(0) { }

Point::Point(int x, int y):
    m_x(x),
    m_y(y) { }

PointF Point::toPointf() const {
    return {
        static_cast<Coord>(m_x),
        static_cast<Coord>(m_y)
    };
}

PointF & Point::toPointf(PointF &point) const {
    point.set(
        static_cast<Coord>(m_x),
        static_cast<Coord>(m_y)
    );

    return point;
}

SDL_Point Point::toSdlPoint() const {
    return SDL_Point{m_x, m_y};
}

SDL_Point& Point::toSdlPoint(SDL_Point& point) const {
    point.x = m_x;
    point.y = m_y;

    return point;
}

SDL_FPoint Point::toSdlFpoint() const {
    return SDL_FPoint{
        float(m_x),
        float(m_y)
    };
}

SDL_FPoint& Point::toSdlFpoint(SDL_FPoint& point) const {
    point.x = float(m_x);
    point.y = float(m_y);

    return point;
}

} // nomad