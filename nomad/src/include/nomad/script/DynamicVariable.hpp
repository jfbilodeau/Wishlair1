//
// Created by jfbil on 2023-06-24.
//

#pragma once

#include "Interpreter.hpp"

#include <functional>

namespace nomad {

using DynamicVariableSetFn = std::function<void(Interpreter*, const ScriptValue&)>;
using DynamicVariableGetFn = std::function<void(Interpreter*, ScriptValue&)> ;

void invalidSetFn(Interpreter* interpreter, ScriptValue& value);
void invalidGetFn(Interpreter* interpreter, const ScriptValue& value);

} // namespace nomad

