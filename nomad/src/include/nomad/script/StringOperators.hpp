//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_STRINGOPERATORS_HPP
#define NOMAD_STRINGOPERATORS_HPP

#include "Interpreter.hpp"

namespace nomad {

void string_operator_load_string(Interpreter* interpreter, NomadString& result);
void string_operator_load_string_variable(Interpreter* interpreter, NomadString& result);
void string_operator_load_format_string(Interpreter* interpreter, NomadString& result);
void string_operator_load_constant(Interpreter* interpreter, NomadString& result);
void string_operator_load_script_variable(Interpreter* interpreter, NomadString& result);
void string_operator_load_dynamic_variable(Interpreter* interpreter, NomadString& result);
void string_operator_load_context_variable(Interpreter* interpreter, NomadString& result);

} // nomad

#endif //NOMAD_STRINGOPERATORS_HPP
