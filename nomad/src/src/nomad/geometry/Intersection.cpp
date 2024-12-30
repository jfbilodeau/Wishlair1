//
// Created by jfbil on 2024-11-14.
//

#include "nomad/geometry/Intersection.hpp"

#include <algorithm>
#include <cmath>

namespace nomad {

bool circle_circle_intersect(const CircleF& a, const CircleF& b) {
    auto dx = a.get_x() - b.get_x();
    auto dy = a.get_y() - b.get_y();
    auto distance_squared = dx * dx + dy * dy;
    auto radius_sum = a.get_radius() + b.get_radius();
    auto radius_sum_squared = radius_sum * radius_sum;

    return distance_squared < radius_sum_squared;
}

bool circle_rectangle_intersect(const CircleF& circle, const RectangleF& rectangle) {
    auto circle_x = circle.get_x();
    auto circle_y = circle.get_y();

    auto closest_x = std::clamp(circle_x, rectangle.get_left(), rectangle.get_right());
    auto closest_y = std::clamp(circle_y, rectangle.get_top(), rectangle.get_bottom());

    auto dx = circle_x - closest_x;
    auto dy = circle_y - closest_y;

    return dx * dx + dy * dy < circle.get_radius() * circle.get_radius();}

bool rectangle_rectangle_intersect(const RectangleF& a, const RectangleF& b) {
    return a.get_left() < b.get_right() &&
           a.get_right() > b.get_left() &&
           a.get_top() < b.get_bottom() &&
           a.get_bottom() > b.get_top();
}

bool point_in_circle(const CircleF& circle, const PointF& point) {
    auto dx = circle.get_x() - point.x();
    auto dy = circle.get_y() - point.y();

    auto distance_squared = dx * dx + dy * dy;
    return distance_squared < circle.get_radius() * circle.get_radius();
}

bool point_in_rectangle(const RectangleF& rectangle, const PointF& point) {
    return point.x() >= rectangle.get_left() &&
           point.x() <= rectangle.get_right() &&
           point.y() >= rectangle.get_top() &&
           point.y() <= rectangle.get_bottom();
}

} // namespace nomad