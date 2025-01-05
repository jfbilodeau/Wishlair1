//
// Created by jfbil on 2024-11-14.
//

#ifndef NOMAD_CIRCLE_HPP
#define NOMAD_CIRCLE_HPP

#include "nomad/nomad.hpp"

#include "nomad/geometry/Point.hpp"

namespace nomad {

class Circle {
public:
    Circle() = default;
    Circle(NomadInteger x, NomadInteger y, NomadInteger radius);

    [[nodiscard]] NomadInteger get_x() const;
    [[nodiscard]] NomadInteger get_y() const;
    [[nodiscard]] NomadInteger get_radius() const;
    [[nodiscard]] const Point& get_center() const;
    [[nodiscard]] NomadInteger get_left() const;
    [[nodiscard]] NomadInteger get_right() const;
    [[nodiscard]] NomadInteger get_top() const;
    [[nodiscard]] NomadInteger get_bottom() const;

    void set_x(NomadInteger x);
    void set_y(NomadInteger y);
    void set_radius(NomadInteger radius);

private:
    Point m_center;
    NomadInteger m_radius = 0;
};

} // namespace nomad

#endif //NOMAD_CIRCLEF_HPP
