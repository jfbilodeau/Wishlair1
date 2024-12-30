//
// Created by jfbilodeau on 23-06-14.
//

#ifndef NOMAD_COMMAND_HPP
#define NOMAD_COMMAND_HPP

#include "nomad/Nomad.hpp"

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
    const Type* return_type;
    NomadDocField;
};


CommandParameterDefinition def_parameter(const NomadString& name, const Type* type, NomadDocArg);

//enum class CommandParameterType {
//    Number,
//    String,
//    Variable,
//    VarArgs,
//    Callback,
//};
//
//const int COMMMAND_PARAMETER_COUNT_ANY = -1;
//
//struct CommandParameter {
//    CommandParameterType type;
//    int callback_parameter_count;
//    NomadDocParamField;
//};
//
//struct CommandDefinition {
//    NomadId id;
//    NomadString name;
//    std::vector<CommandParameter> parameters;
//    bool returns_value;
//    NomadDocField;
//};

using CommandFn = std::function<void(Interpreter*)>;

//CommandParameter def_parameter(CommandParameterType type, NomadDocArg);
//CommandParameter def_callback(int parameter_count, NomadDocArg);

}

#endif //NOMAD_COMMAND_HPP
