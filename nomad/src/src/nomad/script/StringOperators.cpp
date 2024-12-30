//
// Created by jfbil on 2023-06-07.
//

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

#include "nomad/script/StringOperators.hpp"

namespace nomad {

void string_operator_load_string(Interpreter* interpreter, NomadString& result) {
    auto string_id = interpreter->next_id();

    result = interpreter->get_runtime()->get_string(string_id);
}

void string_operator_load_string_variable(Interpreter* interpreter, NomadString& result) {
    auto type = IdentifierType(interpreter->next_id());
    auto context_id = interpreter->next_id();
    auto variable_id = interpreter->next_id();

    auto string_id = to_nomad_id(interpreter->get_variable_value(type, context_id, variable_id));

    result = interpreter->get_runtime()->get_string(string_id);
}

void string_operator_load_format_string(Interpreter* interpreter, NomadString& result) {
    auto format_string_id = interpreter->next_id();

    interpreter->execute_format_string(format_string_id, result);
}

void string_operator_load_constant(Interpreter* interpreter, NomadString& result) {
    auto constant_id = to_nomad_id(interpreter->next_id());

    NomadValue constant_value = interpreter->get_runtime()->get_constant_value(constant_id);

    auto string_id = to_nomad_id(constant_value);

    result = interpreter->get_runtime()->get_string(string_id);
}

void string_operator_load_script_variable(Interpreter* interpreter, NomadString& result) {
    auto variable_id = to_nomad_id(interpreter->next_id());

    auto string_id = to_nomad_id(interpreter->get_script_variable_value(variable_id));

    result = interpreter->get_runtime()->get_string(string_id);
}

void string_operator_load_dynamic_variable(Interpreter* interpreter, NomadString& result) {
    auto variable_id = to_nomad_id(interpreter->next_id());

    auto string_id = to_nomad_id(interpreter->get_runtime()->get_dynamic_variable_value(interpreter, variable_id));

    result = interpreter->get_runtime()->get_string(string_id);
}

void string_operator_load_context_variable(Interpreter* interpreter, NomadString& result) {
    auto context_id = interpreter->next_id();
    auto variable_id = to_nomad_id(interpreter->next_id());

    auto string_id = to_nomad_id(interpreter->get_runtime()->get_context_variable_value(context_id, variable_id));

    result = interpreter->get_runtime()->get_string(string_id);
}

} // nomad