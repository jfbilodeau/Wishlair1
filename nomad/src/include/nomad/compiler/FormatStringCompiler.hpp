//
// Created by jfbil on 2023-06-25.
//

#pragma once

#include "nomad/nomad.hpp"

namespace nomad {

// Forward declarations
class Compiler;
class FormatString;
class Script;

bool compileFormatString(Compiler* compiler, Script* script, const NomadString& format, FormatString* formatString);

} // nomad

