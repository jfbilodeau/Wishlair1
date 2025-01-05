//
// Created by jfbil on 2023-06-25.
//

#ifndef NOMAD_FORMATSTRINGCOMPILER_HPP
#define NOMAD_FORMATSTRINGCOMPILER_HPP

#include "nomad/nomad.hpp"

namespace nomad {

// Forward declarations
class Compiler;
class FormatString;
class Script;

bool compile_format_string(Compiler* compiler, Script* script, const NomadString& format, FormatString* format_string);

} // nomad

#endif //NOMAD_FORMATSTRINGCOMPILER_HPP
