//
// Created by jbilodeau on 2024-09-17.
//

#ifndef NOMAD_FLOAT_HPP
#define NOMAD_FLOAT_HPP

#include "nomad/nomad.hpp"

#include <cmath>

namespace nomad {

// Float operations
inline NomadFloat float_add(NomadFloat a, NomadFloat b) {
    return a + b;
}

inline NomadFloat float_subtract(NomadFloat a, NomadFloat b) {
    return a - b;
}

inline NomadFloat float_multiply(NomadFloat a, NomadFloat b) {
    return a * b;
}

inline NomadFloat float_divide(NomadFloat a, NomadFloat b) {
    return a / b;
}

inline NomadFloat float_modulo(NomadFloat a, NomadFloat b) {
    return std::fmod(a, b);
}

inline NomadFloat float_power(NomadFloat a, NomadFloat b) {
    return std::pow(a, b);
}

inline NomadFloat float_absolute(NomadFloat a) {
    return std::abs(a);
}

inline NomadFloat float_negate(NomadFloat a) {
    return -a;
}

inline NomadFloat float_square_root(NomadFloat a) {
    return std::sqrt(a);
}

inline NomadFloat float_sin(NomadFloat a) {
    return std::sin(a);
}

inline NomadFloat float_cosine(NomadFloat a) {
    return std::cos(a);
}

inline NomadFloat float_tangent(NomadFloat a) {
    return std::tan(a);
}

inline NomadBoolean float_equal_to(NomadFloat a, NomadFloat b) {
    return a == b;
}

inline NomadBoolean float_not_equal_to(NomadFloat a, NomadFloat b) {
    return a != b;
}

inline NomadBoolean float_less_than(NomadFloat a, NomadFloat b) {
    return a < b;
}

inline NomadBoolean float_less_than_or_equal(NomadFloat a, NomadFloat b) {
    return a <= b;
}

inline NomadBoolean float_greater_than(NomadFloat a, NomadFloat b) {
    return a > b;
}

inline NomadBoolean float_greater_than_or_equal(NomadFloat a, NomadFloat b) {
    return a >= b;
}

inline NomadInteger float_to_integer(NomadFloat a) {
    return static_cast<NomadInteger>(a);
}

inline NomadString float_to_string(NomadFloat a) {
    return std::to_string(a);
}

} // namespace nomad

#endif // NOMAD_FLOAT_HPP
