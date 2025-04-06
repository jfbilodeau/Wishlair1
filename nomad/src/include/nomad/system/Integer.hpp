//
// Created by jbilodeau on 2024-09-17.
//

#pragma once

#include "nomad/system/String.hpp"

#include "nomad/nomad.hpp"

#include <cmath>

namespace nomad {

// Integer operations
inline NomadInteger integerAbsolute(NomadInteger a) {
    return std::abs(a);
}

inline NomadInteger integerNegate(NomadInteger a) {
    return -a;
}

inline NomadInteger integerAdd(NomadInteger a, NomadInteger b) {
    return a + b;
}

inline NomadInteger integerSubtract(NomadInteger a, NomadInteger b) {
    return a - b;
}

inline NomadInteger integerMultiply(NomadInteger a, NomadInteger b) {
    return a * b;
}

inline NomadInteger integerDivide(NomadInteger a, NomadInteger b) {
    return a / b;
}

inline NomadInteger integerModulo(NomadInteger a, NomadInteger b) {
    return a % b;
}

inline NomadInteger integerPower(NomadInteger a, NomadInteger b) {
    return static_cast<NomadInteger>(std::pow(a, b));
}

inline NomadInteger integerAnd(NomadInteger a, NomadInteger b) {
    return a & b;
}

inline NomadInteger integerOr(NomadInteger a, NomadInteger b) {
    return a | b;
}

inline NomadInteger integerXor(NomadInteger a, NomadInteger b) {
    return a ^ b;
}

inline NomadBoolean integerEqualTo(NomadInteger a, NomadInteger b) {
    return a == b;
}

inline NomadBoolean integerNotEqualTo(NomadInteger a, NomadInteger b) {
    return a != b;
}

inline NomadBoolean integerLessThan(NomadInteger a, NomadInteger b) {
    return a < b;
}

inline NomadBoolean integerLessThanOrEqual(NomadInteger a, NomadInteger b) {
    return a <= b;
}

inline NomadBoolean integerGreaterThan(NomadInteger a, NomadInteger b) {
    return a > b;
}

inline NomadBoolean integerGreaterThanOrEqual(NomadInteger a, NomadInteger b) {
    return a >= b;
}

inline NomadFloat integerToFloat(NomadInteger a) {
    return static_cast<NomadFloat>(a);
}

inline NomadString integerToString(NomadInteger a) {
    return toString(a);
}

} // nomad

