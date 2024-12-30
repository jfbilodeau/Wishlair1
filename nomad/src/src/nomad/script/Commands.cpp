//
// Created by jfbil on 2023-06-13.
//

#include "nomad/script/Commands.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void command_log_info(Interpreter* interpreter) {
    auto message = interpreter->get_string_parameter(0);

    log::info(message);
}

void command_to_float(Interpreter* interpreter) {
    auto value = interpreter->get_integer_parameter(0);

    auto converted_value = static_cast<NomadFloat>(value);

    interpreter->set_float_result(converted_value);
}

void command_to_integer(Interpreter* interpreter) {
    auto value = interpreter->get_float_parameter(0);

    auto converted_value = static_cast<NomadInteger>(value);

    interpreter->set_integer_result(converted_value);
}

} // nomad