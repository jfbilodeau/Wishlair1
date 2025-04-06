//
// Created by jfbil on 2023-06-13.
//

#include "nomad/script/Commands.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void commandLogInfo(Interpreter* interpreter) {
    auto message = interpreter->getStringParameter(0);

    log::info(message);
}

void commandToFloat(Interpreter* interpreter) {
    auto value = interpreter->getIntegerParameter(0);

    auto converted_value = static_cast<NomadFloat>(value);

    interpreter->setFloatResult(converted_value);
}

void commandToInteger(Interpreter* interpreter) {
    auto value = interpreter->getFloatParameter(0);

    auto converted_value = static_cast<NomadInteger>(value);

    interpreter->setIntegerResult(converted_value);
}

} // nomad