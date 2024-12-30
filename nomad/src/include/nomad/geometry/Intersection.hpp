//
// Created by jfbil on 2024-11-14.
//

#ifndef NOMAD_INTERSECTION_HPP
#define NOMAD_INTERSECTION_HPP

#include "nomad/geometry/CircleF.hpp"
#include "nomad/geometry/RectangleF.hpp"

namespace nomad {

bool circle_circle_intersect(const CircleF& a, const CircleF& b);
bool circle_rectangle_intersect(const CircleF& circle, const RectangleF& rectangle);
bool rectangle_rectangle_intersect(const RectangleF& a, const RectangleF& b);

bool point_in_circle(const CircleF& circle, const PointF& point);
bool point_in_rectangle(const RectangleF& rectangle, const PointF& point);

} // namespace nomad

#endif //NOMAD_INTERSECTION_HPP
