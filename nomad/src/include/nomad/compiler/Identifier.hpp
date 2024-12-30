//
// Created by Jean-François Bilodeau on 2023-06-16.
//

#ifndef NOMAD_IDENTIFIER_HPP
#define NOMAD_IDENTIFIER_HPP

#include "nomad/script/Type.hpp"

namespace nomad {

// Ids for variables types used in opcodes.
const NomadId VARIABLE_TYPE_CONSTANT = -1;
const NomadId VARIABLE_TYPE_DYNAMIC = -2;
const NomadId VARIABLE_TYPE_SCRIPT = -3;

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
    IdentifierType identifier_type = IdentifierType::Unknown;
    const Type* value_type = nullptr;
    NomadId context_id = NOMAD_INVALID_ID;
    union {
        NomadId command_id = NOMAD_INVALID_ID;
        NomadId variable_id;
        NomadId script_id;
    };
};

}

#endif //NOMAD_IDENTIFIER_HPP
