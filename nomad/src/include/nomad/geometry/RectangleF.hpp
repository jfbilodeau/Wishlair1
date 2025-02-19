//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_RECTANGLEF_HPP
#define NOMAD_RECTANGLEF_HPP

#include <SDL.h>

#include "nomad/geometry/PointF.hpp"
#include "nomad/geometry/Geometry.hpp"

namespace nomad {

class RectangleF {
public:
    RectangleF();
    RectangleF(Coord x, Coord y, Coord width, Coord height);
    RectangleF(const RectangleF& other) = default;
    RectangleF(RectangleF&& other) noexcept = default;

    RectangleF& operator=(const RectangleF& other) = default;
    RectangleF& operator=(RectangleF&& other) noexcept = default;

    [[nodiscard]] Coord get_left() const {
        return m_x;
    }

    [[nodiscard]] Coord get_right() const {
        return m_x + m_width;
    }

    [[nodiscard]] Coord get_top() const {
        return m_y;
    }

    [[nodiscard]] Coord get_bottom() const {
        return m_y + m_height;
    }

    [[nodiscard]] Coord get_width() const {
        return m_width;
    }

    [[nodiscard]] Coord get_height() const {
        return m_height;
    }

    [[nodiscard]] bool contains(const PointF& point) const {
        return point.x() >= get_left() && point.x() <= get_right() && point.y() >= get_top() && point.y() <=
                                                                                                        get_bottom();
    }

    [[nodiscard]] bool intersects(const RectangleF& other) const {
        return get_left() < other.get_right() && get_right() > other.get_left() && get_top() < other.get_bottom() &&
                get_bottom() >
                                                                                                                 other.get_top();
    }

    [[nodiscard]] RectangleF intersection(const RectangleF& other) const {
        if (!intersects(other)) {
            return {};
        }

        return {
            std::max(get_left(), other.get_left()),
            std::max(get_top(), other.get_top()),
            std::min(get_right(), other.get_right()) - std::max(get_left(), other.get_left()),
            std::min(get_bottom(), other.get_bottom()) - std::max(get_top(), other.get_top())
        };
    }

    [[nodiscard]] RectangleF union_rect(const RectangleF& other) const {
        return {
            std::min(get_left(), other.get_left()),
            std::min(get_top(), other.get_top()),
            std::max(get_right(), other.get_right()) - std::min(get_left(), other.get_left()),
            std::max(get_bottom(), other.get_bottom()) - std::min(get_top(), other.get_top())
        };
    }

    RectangleF& move(Coord x, Coord y) {
        m_x += x;
        m_y += y;
        return *this;
    }

    RectangleF& move(const PointF& point) {
        return move(point.x(), point.y());
    }

    RectangleF& resize(Coord width, Coord height) {
        m_width += width;
        m_height += height;
        return *this;
    }

    RectangleF& resize(const PointF& point) {
        return resize(point.x(), point.y());
    }

    RectangleF& set_left(Coord left) {
        m_x = left;
        return *this;
    }

    RectangleF& set_right(Coord right) {
        m_width = right - m_x;
        return *this;
    }

    RectangleF& set_top(Coord top) {
        m_y = top;
        return *this;
    }

    RectangleF& set_bottom(Coord bottom) {
        m_height = bottom - m_y;
        return *this;
    }

    RectangleF& set_width(Coord width) {
        m_width = width;
        return *this;
    }

    RectangleF& set_height(Coord height) {
        m_height = height;
        return *this;
    }

    RectangleF& set_position(Coord x, Coord y) {
        m_x = x;
        m_y = y;
        return *this;
    }

    RectangleF& set_position(const PointF& point) {
        return set_position(point.x(), point.y());
    }

    RectangleF& set_size(Coord width, Coord height) {
        m_width = width;
        m_height = height;
        return *this;
    }

    RectangleF& set_size(const PointF& point) {
        return set_size(point.x(), point.y());
    }

    RectangleF& set(Coord x, Coord y, Coord width, Coord height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        return *this;
    }

    RectangleF& set(const PointF& point, const PointF& size) {
        return set(point.x(), point.y(), size.x(), size.y());
    }

    RectangleF& set(const RectangleF& other) {
        return set(other.m_x, other.m_y, other.m_width, other.m_height);
    }

    RectangleF& set(const SDL_Rect& rect) {
        return set(rect.x, rect.y, rect.w, rect.h);
    }

    RectangleF& set(const SDL_FRect& rect) {
        return set(rect.x, rect.y, rect.w, rect.h);
    }

    [[nodiscard]] SDL_Rect to_sdl_rect() const;
    [[nodiscard]] SDL_FRect to_sdl_frect() const;

private:
    Coord m_x;
    Coord m_y;
    Coord m_width;
    Coord m_height;
};

} // nomad

#endif //NOMAD_RECTANGLEF_HPP
