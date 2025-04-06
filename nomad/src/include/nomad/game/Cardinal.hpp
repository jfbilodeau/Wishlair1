//
// Created by jfbil on 2024-10-26.
//

#pragma once

#include "nomad/nomad.hpp"

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

const NomadString& toString(NomadString cardinal);
Cardinal fromString(const NomadString& cardinal_name);

} // namespace nomad
