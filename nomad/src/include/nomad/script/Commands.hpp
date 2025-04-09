//
// Created by jfbil on 2023-06-13.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/script/Interpreter.hpp"

namespace nomad {

void registerBuildInCommands(Runtime* runtime);
// void commandLogInfo(Interpreter* interpreter);
// void commandToFloat(Interpreter* interpreter);
// void commandToInteger(Interpreter* interpreter);

} // nomad

