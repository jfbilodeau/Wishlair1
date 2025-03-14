//
// Created by jfbil on 2023-06-07.
//

#pragma once

#include "nomad/geometry/Geometry.hpp"

#include <SDL.h>

#include <cmath>

namespace nomad {

class PointF {
public:
    PointF();
    PointF(Coord x, Coord y);
    PointF(const PointF& other) = default;
    PointF(PointF&& other) noexcept = default;

    PointF& operator=(const PointF& other) = default;
    PointF& operator=(PointF&& other) noexcept = default;

    [[nodiscard]] Coord x() const {
        return m_x;
    }

    [[nodiscard]] Coord y() const {
        return m_y;
    }

    void set_x(Coord x) {
        m_x = x;
    }

    void set_y(Coord y) {
        m_y = y;
    }

    void set(Coord x, Coord y) {
        m_x = x;
        m_y = y;
    }

    void set(const PointF& other) {
        m_x = other.m_x;
        m_y = other.m_y;
    }

    void zero() {
        m_x = 0;
        m_y = 0;
    }

    [[nodiscard]]
    NomadBoolean is_zero() const {
        return m_x == 0 && m_y == 0;
    }

    void translate(Coord delta_x, Coord delta_y) {
        m_x += delta_x;
        m_y += delta_y;
    }

    void translate(const PointF& delta) {
        m_x += delta.m_x;
        m_y += delta.m_y;
    }

    void normalize() {
        auto length = std::sqrt(m_x * m_x + m_y * m_y);
        m_x /= length;
        m_y /= length;
    }

    [[nodiscard]] NomadFloat length() const {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }

    [[nodiscard]] NomadFloat length_squared() const {
        return m_x * m_x + m_y * m_y;
    }

    [[nodiscard]] NomadFloat dot(const PointF& other) const {
        return m_x * other.m_x + m_y * other.m_y;
    }

    [[nodiscard]] NomadFloat cross(const PointF& other) const {
        return m_x * other.m_y - m_y * other.m_x;
    }

    [[nodiscard]] PointF perpendicular() const {
        return {-m_y, m_x};
    }

    [[nodiscard]] PointF normal() const {
        return *this / length();
    }

    [[nodiscard]] PointF lerp(const PointF& other, NomadFloat t) const {
        return {m_x + (other.m_x - m_x) * t, m_y + (other.m_y - m_y) * t};
    }

    [[nodiscard]] PointF project(const PointF& other) const {
        auto length_squared = other.length_squared();
        if (length_squared == 0) {
            return {0, 0};
        }

        auto dot_product = dot(other);
        return other * (dot_product / length_squared);
    }

    [[nodiscard]] NomadFloat distance_to(const PointF& other) const;
    [[nodiscard]] NomadFloat distance_to(Coord x, Coord y) const;

    [[nodiscard]] SDL_Point to_sdl_point() const;
    SDL_Point& to_sdl_point(SDL_Point& point) const;
    [[nodiscard]] SDL_FPoint to_sdl_fpoint() const;
    SDL_FPoint& to_sdl_fpoint(SDL_FPoint& point) const;

    [[nodiscard]] PointF operator-() const {
        return {-m_x, -m_y};
    }

    [[nodiscard]] PointF operator+(const PointF& other) const {
        return {m_x + other.m_x, m_y + other.m_y};
    }

    [[nodiscard]] PointF operator-(const PointF& other) const {
        return {m_x - other.m_x, m_y - other.m_y};
    }

    [[nodiscard]] PointF operator*(NomadFloat scalar) const {
        return {m_x * scalar, m_y * scalar};
    }

    [[nodiscard]] PointF operator/(NomadFloat scalar) const {
        return {m_x / scalar, m_y / scalar};
    }

    [[nodiscard]]
    NomadFloat angle_to(PointF destination) const {
        // Calculate the angle between this point and `destination`
        auto delta_x = destination.x() - x();
        auto delta_y = destination.y() - y();

        return std::atan2(delta_y, delta_x);
    }

private:
    Coord m_x;
    Coord m_y;
};

static bool operator==(const PointF& lhs, const PointF& rhs) {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

static bool operator!=(const PointF& lhs, const PointF& rhs) {
    return !(lhs == rhs);
}

static PointF operator+(const PointF& lhs, const PointF& rhs) {
    return {lhs.x() + rhs.x(), lhs.y() + rhs.y()};
}

static PointF operator-(const PointF& lhs, const PointF& rhs) {
    return {lhs.x() - rhs.x(), lhs.y() - rhs.y()};
}

static PointF operator*(const PointF& lhs, NomadFloat scalar) {
    return {lhs.x() * scalar, lhs.y() * scalar};
}

static PointF operator*(NomadFloat scalar, const PointF& rhs) {
    return { rhs.x() * scalar, rhs.y() * scalar };
}

static PointF operator/(const PointF& lhs, NomadFloat scalar) {
    return { lhs.x() / scalar, lhs.y() / scalar };
}

} // nomad

