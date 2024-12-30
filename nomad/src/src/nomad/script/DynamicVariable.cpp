//
// Created by jfbil on 2023-06-24.
//

#include "nomad/script/DynamicVariable.hpp"

#include "nomad/script/Variable.hpp"
#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void invalid_set_fn(Interpreter* interpreter, ScriptValue& value) {
    log::warning("Cannot set variable");
}

void invalid_get_fn(Interpreter* interpreter, const ScriptValue& value) {
    log::warning("Cannot get variable");
}

} // namespace nomad