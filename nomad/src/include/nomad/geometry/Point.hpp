//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_POINT_HPP
#define NOMAD_POINT_HPP

#include <SDL.h>

namespace nomad {

class Point {
public:
    Point();
    Point(int x, int y);
    Point(const Point& other) = default;
    Point(Point&& other) noexcept = default;

    Point& operator=(const Point& other) = default;
    Point& operator=(Point&& other) noexcept = default;

    [[nodiscard]] int x() const {
        return m_x;
    }

    [[nodiscard]] int y() const {
        return m_y;
    }

    void set_x(int x) {
        m_x = x;
    }

    void set_y(int y) {
        m_y = y;
    }

    void set(int x, int y) {
        m_x = x;
        m_y = y;
    }

    void set(const Point& other) {
        m_x = other.m_x;
        m_y = other.m_y;
    }

    void move(int dx, int dy) {
        m_x += dx;
        m_y += dy;
    }

    void move(const Point& delta) {
        m_x += delta.m_x;
        m_y += delta.m_y;
    }

    [[nodiscard]] SDL_Point to_sdl_point() const;
    SDL_Point& to_sdl_point(SDL_Point& point) const;
    [[nodiscard]] SDL_FPoint to_sdl_fpoint() const;
    SDL_FPoint& to_sdl_fpoint(SDL_FPoint& point) const;

private:
    int m_x;
    int m_y;
};

static bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

static bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}

} // nomad

#endif //NOMAD_POINT_HPP
