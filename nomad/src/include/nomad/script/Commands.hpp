//
// Created by jfbil on 2023-06-13.
//

#ifndef NOMAD_COMMANDS_HPP
#define NOMAD_COMMANDS_HPP

#include "nomad/Nomad.hpp"

#include "nomad/script/Interpreter.hpp"

namespace nomad {

void command_log_info(Interpreter* interpreter);
void command_to_float(Interpreter* interpreter);
void command_to_integer(Interpreter* interpreter);

} // nomad

#endif //NOMAD_COMMANDS_HPP
