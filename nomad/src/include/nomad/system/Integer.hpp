//
// Created by jbilodeau on 2024-09-17.
//

#ifndef NOMAD_INTEGER_HPP
#define NOMAD_INTEGER_HPP

#include "nomad/system/String.hpp"

#include "nomad/Nomad.hpp"

#include <cmath>

namespace nomad {

// Integer operations
inline NomadInteger integer_absolute(NomadInteger a) {
    return std::abs(a);
}

inline NomadInteger integer_negate(NomadInteger a) {
    return -a;
}

inline NomadInteger integer_add(NomadInteger a, NomadInteger b) {
    return a + b;
}

inline NomadInteger integer_subtract(NomadInteger a, NomadInteger b) {
    return a - b;
}

inline NomadInteger integer_multiply(NomadInteger a, NomadInteger b) {
    return a * b;
}

inline NomadInteger integer_divide(NomadInteger a, NomadInteger b) {
    return a / b;
}

inline NomadInteger integer_modulo(NomadInteger a, NomadInteger b) {
    return a % b;
}

inline NomadInteger integer_power(NomadInteger a, NomadInteger b) {
    return static_cast<NomadInteger>(std::pow(a, b));
}

inline NomadInteger integer_and(NomadInteger a, NomadInteger b) {
    return a & b;
}

inline NomadInteger integer_or(NomadInteger a, NomadInteger b) {
    return a | b;
}

inline NomadInteger integer_xor(NomadInteger a, NomadInteger b) {
    return a ^ b;
}

inline NomadBoolean integer_equal_to(NomadInteger a, NomadInteger b) {
    return a == b;
}

inline NomadBoolean integer_not_equal_to(NomadInteger a, NomadInteger b) {
    return a != b;
}

inline NomadBoolean integer_less_than(NomadInteger a, NomadInteger b) {
    return a < b;
}

inline NomadBoolean integer_less_than_or_equal(NomadInteger a, NomadInteger b) {
    return a <= b;
}

inline NomadBoolean integer_greater_than(NomadInteger a, NomadInteger b) {
    return a > b;
}

inline NomadBoolean integer_greater_than_or_equal(NomadInteger a, NomadInteger b) {
    return a >= b;
}

inline NomadFloat integer_to_float(NomadInteger a) {
    return static_cast<NomadFloat>(a);
}

inline NomadString integer_to_string(NomadInteger a) {
    return to_string(a);
}

} // nomad

#endif // NOMAD_INTEGER_HPP
