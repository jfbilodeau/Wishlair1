//
// Created by jfbil on 2023-06-07.
//

#pragma once

#include <SDL2/SDL_rect.h>

namespace nomad {

class PointF;

class Point {
public:
    Point();
    Point(int x, int y);
    Point(const Point& other) = default;
    Point(Point&& other) noexcept = default;

    Point& operator=(const Point& other) = default;
    Point& operator=(Point&& other) noexcept = default;

    [[nodiscard]] int getX() const {
        return m_x;
    }

    [[nodiscard]] int getY() const {
        return m_y;
    }

    void setX(int x) {
        m_x = x;
    }

    void setY(int y) {
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

    [[nodiscard]] PointF toPointf() const;
    PointF& toPointf(PointF& point) const;
    [[nodiscard]] SDL_Point toSdlPoint() const;
    SDL_Point& toSdlPoint(SDL_Point& point) const;
    [[nodiscard]] SDL_FPoint toSdlFpoint() const;
    SDL_FPoint& toSdlFpoint(SDL_FPoint& point) const;

private:
    int m_x;
    int m_y;
};

static bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.getX() == rhs.getX() && lhs.getY() == rhs.getY();
}

static bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}

} // nomad

