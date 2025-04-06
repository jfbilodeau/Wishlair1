//
// Created by jfbilodeau on 23-06-14.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/script/Documentation.hpp"
#include "nomad/script/Interpreter.hpp"

#include <functional>
#include <vector>

namespace nomad {

using CommandFn = std::function<void(Interpreter*)>;

struct CommandParameterDefinition {
    NomadString name;
    const Type* type;
    NomadDocField;
};

struct CommandDefinition {
    NomadId id;
    NomadString name;
    CommandFn fn;
    std::vector<CommandParameterDefinition> parameters;
    const Type* returnType;
    NomadDocField;
};


CommandParameterDefinition defParameter(const NomadString& name, const Type* type, NomadDocArg);

using CommandFn = std::function<void(Interpreter*)>;

}

