//
// Created by jfbil on 2024-11-29.
//

#include "nomad/game/Body.hpp"

#include "nomad/geometry/CircleF.hpp"
#include "nomad/geometry/RectangleF.hpp"
#include "nomad/geometry/Intersection.hpp"

#include "nomad/log/Logger.hpp"

namespace nomad {

Body::Body(NomadFloat x, NomadFloat y):
    m_shape(BodyShape::None),
    m_location(x, y)
{
}

Body::Body(NomadFloat x, NomadFloat y, NomadFloat width, NomadFloat height):
    m_shape(BodyShape::Rectangle),
    m_location(x, y),
    m_width(width),
    m_height(height)
{
}

Body::Body(NomadFloat x, NomadFloat y, NomadFloat radius):
    m_shape(BodyShape::Circle),
    m_location(x, y),
    m_radius(radius)
{
}

BodyShape Body::get_shape() const {
    return m_shape;
}

BodyType Body::get_type() const {
    return m_type;
}

NomadFloat Body::get_x() const {
    return m_location.x();
}

void Body::set_x(NomadFloat x) {
    m_location.set_x(x);
}

NomadFloat Body::get_y() const {
    return m_location.y();
}

void Body::set_y(NomadFloat y) {
    m_location.set_y(y);
}

const PointF& Body::get_location() const {
    return m_location;
}

void Body::set_location(NomadFloat x, NomadFloat y) {
    m_location.set(x, y);
}

void Body::set_location(const PointF& location) {
    m_location.set(location);
}

void Body::translate(NomadFloat delta_x, NomadFloat delta_y) {
    m_location.translate(delta_x, delta_y);
}

void Body::translate(const PointF& delta) {
    m_location.translate(delta);
}

NomadFloat Body::get_width() const {
    return m_width;
}

NomadFloat Body::get_height() const {
    return m_height;
}

NomadFloat Body::get_radius() const {
    return m_radius;
}

void Body::set_no_body() {
    m_shape = BodyShape::None;
}

void Body::set_rectangle_body(BodyType type, NomadFloat width, NomadFloat height) {
    m_type = type;
    m_shape = BodyShape::Rectangle;
    m_width = width;
    m_height = height;
}

void Body::set_circle_body(BodyType type, NomadFloat radius) {
    m_type = type;
    m_shape = BodyShape::Circle;
    m_radius = radius;
}

bool Body::contains(const PointF& point) const {
    switch (m_shape) {
        case BodyShape::None:
            return false;

        case BodyShape::Rectangle:
            return point.x() >= m_location.x() &&
                   point.x() <= m_location.x() + m_width &&
                   point.y() >= m_location.y() &&
                   point.y() <= m_location.y() + m_height;

        case BodyShape::Circle:
            auto dx = m_location.x() - point.x();
            auto dy = m_location.y() - point.y();
            auto distance_squared = dx * dx + dy * dy;
            return distance_squared < m_radius * m_radius;
    }

    log::warning("[Body::contains] Unexpected body shape: " + std::to_string(static_cast<int>(m_shape)));
    return false;
}

bool Body::intersects(const Body& other) const {
    switch (m_shape) {
        case BodyShape::None:
            return false;

        case BodyShape::Rectangle:
            switch (other.m_shape) {
                case BodyShape::None:
                    return false;

                case BodyShape::Rectangle:
                    return rectangle_rectangle_intersect(
                        RectangleF(
                            m_location.x(),
                            m_location.y(),
                            m_width,
                            m_height
                        ),
                        RectangleF(
                            other.m_location.x(),
                            other.m_location.y(),
                            other.m_width,
                            other.m_height)
                        );

                case BodyShape::Circle:
                    return circle_rectangle_intersect(
                        CircleF(
                            other.m_location.x(),
                            other.m_location.y(),
                            other.m_radius
                        ),
                        RectangleF(
                            m_location.x(),
                            m_location.y(),
                            m_width,
                            m_height
                        )
                    );
            }

        case BodyShape::Circle:
            switch (other.m_shape) {
                case BodyShape::None:
                    return false;

                case BodyShape::Rectangle:
                    return circle_rectangle_intersect(
                        CircleF(
                            m_location.x(),
                            m_location.y(),
                            m_radius
                        ),
                        RectangleF(
                            other.m_location.x(),
                            other.m_location.y(),
                            other.m_width,
                            other.m_height
                        )
                    );

                case BodyShape::Circle:
                    return circle_circle_intersect(
                        CircleF(
                            m_location.x(),
                            m_location.y(),
                            m_radius
                        ),
                        CircleF(
                            other.m_location.x(),
                            other.m_location.y(),
                            other.m_radius
                        )
                    );
            }
    }

    log::warning("[Body::intersects] Unexpected body shape: " + std::to_string(static_cast<int>(m_shape)));
    return false;
}

bool Body::intersects(const RectangleF& rectangle) const {
    switch (m_shape) {
        case BodyShape::None:
            return false;

        case BodyShape::Rectangle:
            return rectangle_rectangle_intersect(rectangle, RectangleF(m_location.x(), m_location.y(), m_width, m_height));

        case BodyShape::Circle:
            return circle_rectangle_intersect(CircleF(m_location.x(), m_location.y(), m_radius), rectangle);
    }

    log::warning("[Body::intersects] Unexpected body shape: " + std::to_string(static_cast<int>(m_shape)));
    return false;
}

bool Body::intersects(const CircleF& circle) const {
    switch (m_shape) {
        case BodyShape::None:
            return false;

        case BodyShape::Rectangle:
            return circle_rectangle_intersect(circle, RectangleF(m_location.x(), m_location.y(), m_width, m_height));

        case BodyShape::Circle:
            return circle_circle_intersect(CircleF(m_location.x(), m_location.y(), m_radius), circle);
    }

    log::warning("[Body::intersects] Unexpected body shape: " + std::to_string(static_cast<int>(m_shape)));
    return false;
}

} // namespace nomad