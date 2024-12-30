//
// Created by jfbil on 2023-09-29.
//

#ifndef NOMAD_DECOMPILER_HPP
#define NOMAD_DECOMPILER_HPP

#include "nomad/Nomad.hpp"

namespace nomad {

// Forward declarations
class Runtime;
class Script;

NomadString decompile(Runtime* runtime, Script* script);

} // nomad

#endif //NOMAD_DECOMPILER_HPP
