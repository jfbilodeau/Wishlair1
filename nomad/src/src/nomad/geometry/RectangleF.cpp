//
// Created by jfbil on 2023-06-07.
//

#include "nomad/geometry/RectangleF.hpp"

namespace nomad {

RectangleF::RectangleF():
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0) { }

RectangleF::RectangleF(Coord x, Coord y, Coord width, Coord height):
    m_x(x),
    m_y(y),
    m_width(width),
    m_height(height) { }

SDL_Rect RectangleF::toSdlRect() const {
    return SDL_Rect{
        int(m_x),
        int(m_y),
        int(m_width),
        int(m_height)
    };
}

SDL_FRect RectangleF::toSdlFrect() const {
    return SDL_FRect{
        float(m_x),
        float(m_y),
        float(m_width),
        float(m_height)
    };
}

} // nomad