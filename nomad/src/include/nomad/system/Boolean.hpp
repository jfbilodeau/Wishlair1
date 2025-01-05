//
// Created by jbilodeau on 2024-09-18.
//

#ifndef NOMAD_BOOLEAN_HPP
#define NOMAD_BOOLEAN_HPP

#include "nomad/nomad.hpp"

namespace nomad {

// Boolean operations
inline NomadBoolean boolean_and(NomadBoolean a, NomadBoolean b) {
    return a && b;
}

inline NomadBoolean boolean_or(NomadBoolean a, NomadBoolean b) {
    return a || b;
}

inline NomadBoolean boolean_not(NomadBoolean a) {
    return !a;
}

inline NomadBoolean boolean_equal_to(NomadBoolean a, NomadBoolean b) {
    return a == b;
}

inline NomadBoolean boolean_not_equal_to(NomadBoolean a, NomadBoolean b) {
    return a != b;
}

} // namespace nomad

#endif // NOMAD_BOOLEAN_HPP
