//
// Created by jfbil on 2023-06-24.
//

#ifndef NOMAD_DYNAMICVARIABLE_HPP
#define NOMAD_DYNAMICVARIABLE_HPP

#include "Interpreter.hpp"

#include <functional>

namespace nomad {

using DynamicVariableSetFn = std::function<void(Interpreter*, const ScriptValue&)>;
using DynamicVariableGetFn = std::function<void(Interpreter*, ScriptValue&)> ;

void invalid_set_fn(Interpreter* interpreter, ScriptValue& value);
void invalid_get_fn(Interpreter* interpreter, const ScriptValue& value);

} // namespace nomad

#endif //NOMAD_DYNAMICVARIABLE_HPP
