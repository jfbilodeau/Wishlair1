//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_RECTANGLE_HPP
#define NOMAD_RECTANGLE_HPP

#include <SDL.h>

#include "nomad/geometry/Point.hpp"
#include "nomad/geometry/Geometry.hpp"

namespace nomad {

class Rectangle {
public:
    Rectangle();
    Rectangle(NomadInteger x, NomadInteger y, NomadInteger width, NomadInteger height);
    Rectangle(const Rectangle& other) = default;
    Rectangle(Rectangle&& other) noexcept = default;

    Rectangle& operator=(const Rectangle& other) = default;
    Rectangle& operator=(Rectangle&& other) noexcept = default;

    [[nodiscard]] NomadInteger get_left() const {
        return m_x;
    }

    [[nodiscard]] NomadInteger get_right() const {
        return m_x + m_width;
    }

    [[nodiscard]] NomadInteger get_top() const {
        return m_y;
    }

    [[nodiscard]] NomadInteger get_bottom() const {
        return m_y + m_height;
    }

    [[nodiscard]] NomadInteger get_width() const {
        return m_width;
    }

    [[nodiscard]] NomadInteger get_height() const {
        return m_height;
    }

    [[nodiscard]] bool contains(const Point& point) const {
        return point.x() >= get_left() && point.x() <= get_right() && point.y() >= get_top() && point.y() <=
                                                                                                        get_bottom();
    }

    [[nodiscard]] bool Intersects(const Rectangle& other) const {
        return get_left() < other.get_right() && get_right() > other.get_left() && get_top() < other.get_bottom() &&
                get_bottom() >
                                                                                                                 other.get_top();
    }

    [[nodiscard]] Rectangle NomadIntegerersection(const Rectangle& other) const {
        if (!Intersects(other)) {
            return {};
        }

        return {
            std::max(get_left(), other.get_left()),
            std::max(get_top(), other.get_top()),
            std::min(get_right(), other.get_right()) - std::max(get_left(), other.get_left()),
            std::min(get_bottom(), other.get_bottom()) - std::max(get_top(), other.get_top())
        };
    }

    [[nodiscard]] Rectangle union_rect(const Rectangle& other) const {
        return {
            std::min(get_left(), other.get_left()),
            std::min(get_top(), other.get_top()),
            std::max(get_right(), other.get_right()) - std::min(get_left(), other.get_left()),
            std::max(get_bottom(), other.get_bottom()) - std::min(get_top(), other.get_top())
        };
    }

    Rectangle& move(NomadInteger x, NomadInteger y) {
        m_x += x;
        m_y += y;
        return *this;
    }

    Rectangle& move(const Point& point) {
        return move(point.x(), point.y());
    }

    Rectangle& resize(NomadInteger width, NomadInteger height) {
        m_width += width;
        m_height += height;
        return *this;
    }

    Rectangle& resize(const Point& point) {
        return resize(point.x(), point.y());
    }

    Rectangle& set_left(NomadInteger left) {
        m_x = left;
        return *this;
    }

    Rectangle& set_right(NomadInteger right) {
        m_width = right - m_x;
        return *this;
    }

    Rectangle& set_top(NomadInteger top) {
        m_y = top;
        return *this;
    }

    Rectangle& set_bottom(NomadInteger bottom) {
        m_height = bottom - m_y;
        return *this;
    }

    Rectangle& set_width(NomadInteger width) {
        m_width = width;
        return *this;
    }

    Rectangle& set_height(NomadInteger height) {
        m_height = height;
        return *this;
    }

    Rectangle& set_position(NomadInteger x, NomadInteger y) {
        m_x = x;
        m_y = y;
        return *this;
    }

    Rectangle& set_position(const Point& point) {
        return set_position(point.x(), point.y());
    }

    Rectangle& set_size(NomadInteger width, NomadInteger height) {
        m_width = width;
        m_height = height;
        return *this;
    }

    Rectangle& set_size(const Point& point) {
        return set_size(point.x(), point.y());
    }

    Rectangle& set(NomadInteger x, NomadInteger y, NomadInteger width, NomadInteger height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        return *this;
    }

    Rectangle& set(const Point& point, const Point& size) {
        return set(point.x(), point.y(), size.x(), size.y());
    }

    Rectangle& set(const Rectangle& other) {
        return set(other.m_x, other.m_y, other.m_width, other.m_height);
    }

    Rectangle& set(const SDL_Rect& rect) {
        return set(rect.x, rect.y, rect.w, rect.h);
    }

    Rectangle& set(const SDL_FRect& rect);

    [[nodiscard]] SDL_Rect to_sdl_rect() const;
    [[nodiscard]] SDL_FRect to_sdl_frect() const;

private:
    NomadInteger m_x;
    NomadInteger m_y;
    NomadInteger m_width;
    NomadInteger m_height;
};

} // nomad

#endif //NOMAD_RECTANGLE_HPP
