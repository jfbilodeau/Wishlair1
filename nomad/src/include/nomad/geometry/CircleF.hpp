//
// Created by jfbil on 2024-11-14.
//

#ifndef NOMAD_CIRCLEF_HPP
#define NOMAD_CIRCLEF_HPP

#include "nomad/Nomad.hpp"

#include "nomad/geometry/PointF.hpp"

namespace nomad {

class CircleF {
public:
    CircleF() = default;
    CircleF(NomadFloat x, NomadFloat y, NomadFloat radius);

    [[nodiscard]] NomadFloat get_x() const;
    [[nodiscard]] NomadFloat get_y() const;
    [[nodiscard]] NomadFloat get_radius() const;
    [[nodiscard]] const PointF& get_center() const;
    [[nodiscard]] NomadFloat get_left() const;
    [[nodiscard]] NomadFloat get_right() const;
    [[nodiscard]] NomadFloat get_top() const;
    [[nodiscard]] NomadFloat get_bottom() const;

    void set_x(NomadFloat x);
    void set_y(NomadFloat y);
    void set_radius(NomadFloat radius);

private:
    PointF m_center;
    NomadFloat m_radius = 0;
};

} // namespace nomad

#endif //NOMAD_CIRCLEF_HPP
