//
// Created by jfbil on 2024-10-26.
//

#ifndef NOMAD_CARDINAL_HPP
#define NOMAD_CARDINAL_HPP

#include "nomad/Nomad.hpp"

namespace nomad {

enum class Cardinal {
    Unknown = 1,
    North,
    South,
    East,
    West,
};

namespace Cardinals {
const NomadString Unknown = "unknown";
const NomadString North = "north";
const NomadString South = "south";
const NomadString East = "east";
const NomadString West = "west";
}

const NomadString& to_string(Cardinal cardinal);
Cardinal from_string(const NomadString& cardinal_name);

} // namespace nomad

#endif //NOMAD_CARDINAL_HPP
