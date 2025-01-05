//
// Created by jfbil on 2024-11-29.
//

#ifndef NOMAD_BODY_HPP
#define NOMAD_BODY_HPP

#include "nomad/nomad.hpp"

#include "nomad/geometry/PointF.hpp"

//#include "chipmunk/chipmunk.h"

namespace nomad {

// Forward declarations
class CircleF;
class RectangleF;

enum class BodyShape {
    None = 1,
    Rectangle,
    Circle,
};

enum class BodyType {
    Static = 1,
    Dynamic,
    Kinematic,
};

class Body {
public:
    Body() = default;
    Body(const Body& other) = default;
    Body(NomadFloat x, NomadFloat y);
    Body(NomadFloat x, NomadFloat y, NomadFloat width, NomadFloat height);
    Body(NomadFloat x, NomadFloat y, NomadFloat radius);

    ~Body() = default;

    [[nodiscard]] BodyShape get_shape() const;

    [[nodiscard]] BodyType get_type() const;

    [[nodiscard]] NomadFloat get_x() const;
    void set_x(NomadFloat x);

    [[nodiscard]] NomadFloat get_y() const;
    void set_y(NomadFloat y);

    [[nodiscard]] const PointF& get_location() const;
    void set_location(NomadFloat x, NomadFloat y);
    void set_location(const PointF& location);

    void translate(NomadFloat delta_x, NomadFloat delta_y);
    void translate(const PointF& delta);

    [[nodiscard]] NomadFloat get_width() const;

    [[nodiscard]] NomadFloat get_height() const;

    [[nodiscard]] NomadFloat get_radius() const;

    void set_no_body();
    void set_rectangle_body(BodyType type, NomadFloat width, NomadFloat height);
    void set_circle_body(BodyType type, NomadFloat radius);

    [[nodiscard]] bool contains(const PointF& point) const;
    [[nodiscard]] bool intersects(const Body& other) const;
    [[nodiscard]] bool intersects(const RectangleF& rectangle) const;
    [[nodiscard]] bool intersects(const CircleF& circle) const;

//    void pre_simulation(cpSpace* space);
//    void post_simulation(cpSpace* space);

    Body& operator=(const Body& other) = default;

private:
    BodyShape m_shape = BodyShape::None;
    BodyType m_type = BodyType::Static;
    PointF m_location = {};
    NomadFloat m_width = 0.0f;
    NomadFloat m_height = 0.0f;
    NomadFloat m_radius = 0.0f;

    NomadBoolean m_body_valid = false;
//    cpBody* m_cp_body = nullptr;
//    cpShape* m_cp_shape = nullptr;
};

} // namespace nomad

#endif //NOMAD_BODY_HPP
