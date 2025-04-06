//
// Created by Jean-François Bilodeau on 2023-06-16.
//

#pragma once

#include "nomad/script/Type.hpp"

namespace nomad {

// Ids for variables types used in opcodes.
constexpr NomadId VARIABLE_TYPE_CONSTANT = -1;
constexpr NomadId VARIABLE_TYPE_DYNAMIC = -2;
constexpr NomadId VARIABLE_TYPE_SCRIPT = -3;

enum class IdentifierType {
    Unknown = 1,
    Keyword,
    Statement,
    Command,
    Script,
    Constant,
    DynamicVariable,
    ContextVariable,
    ScriptVariable,
    Parameter,
};

struct IdentifierDefinition {
    IdentifierType identifierType = IdentifierType::Unknown;
    const Type* valueType = nullptr;
    NomadId contextId = NOMAD_INVALID_ID;
    union {
        NomadId commandId = NOMAD_INVALID_ID;
        NomadId variableId;
        NomadId scriptId;
    };
};

}

