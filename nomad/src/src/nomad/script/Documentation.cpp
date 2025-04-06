//
// Created by jfbil on 2023-06-26.
//

#include "nomad/compiler/Compiler.hpp"
#include "nomad/script/Documentation.hpp"
#include "nomad/compiler/Parser.hpp"

#include "nomad/script/Runtime.hpp"

#include <boost/json.hpp>

namespace nomad {

#if defined(NOMAD_SCRIPT_DOC)

void generateDocumentation(Runtime* runtime, std::ostream& out) {

    boost::json::object root;

    // Keywords.
    auto& keywords_object = root["keywords"].emplace_object();

    std::vector<KeywordDefinition> keywords;

    runtime->getKeywords(keywords);

    for (const KeywordDefinition& keyword : keywords) {
        auto& keyword_object = keywords_object[keyword.keyword].emplace_object();
        keyword_object["doc"] = keyword.doc;
    }

    // Instructions
//    std::vector<OpCodeDefinition> op_codes;
//
//    runtime->get_opcode_definitions(op_codes);
//
//    auto& instructions_object = root["instructions"].emplace_object();
//
//    for (auto& op_code : op_codes) {
//        auto& instruction_object = instructions_object[op_code.name].emplace_object();
//        instruction_object["name"] = op_code.name;
//        instruction_object["id"] = op_code.id;
//        instruction_object["doc"] = op_code.doc;
//    }

    // Operators
//    std::vector<BinaryOperator> binary_operators;
//
//    runtime->get_operators(binary_operators);
//
//    auto& operators_object = root["operators"].emplace_object();
//
//    for (const OperatorDefinition& operator_ : operators) {
//        auto& operator_object = operators_object[operator_.name].emplace_object();
//        operator_object["name"] = operator_.name;
//        operator_object["id"] = operator_.id;
//    }
//
//    // String operators
//    std::vector<StringOperatorDefinition> string_operators;
//
//    runtime->get_string_operators(string_operators);
//
//    auto& string_operators_object = root["string_operators"].emplace_object();
//
//    for (const StringOperatorDefinition& string_operator : string_operators) {
//        auto& string_operator_object = string_operators_object[string_operator.name].emplace_object();
//        string_operator_object["name"] = string_operator.name;
//        string_operator_object["id"] = string_operator.id;
//    }

    // Commands.
    std::vector<CommandDefinition> commands;

    runtime->getCommands(commands);

    auto& commands_object = root["commands"].emplace_object();

    for (const CommandDefinition& command : commands) {
        auto& command_object = commands_object[command.name].emplace_object();

        command_object["name"] = command.name;
        command_object["id"] = command.id;
        command_object["doc"] = command.doc;
        auto& parameters_array = command_object["parameters"].emplace_array();

        for (auto& parameter : command.parameters) {
            NomadString parameter_type = parameter.type->getName();

            auto& parameter_object = command_object["parameters"].emplace_object();

            parameter_object["type"] = parameter_type;
            parameter_object["doc"] = parameter.doc;
        }
    }

    out << boost::json::serialize(root);
}

void generateKeywords(Runtime* runtime, std::ostream& out) {
    out << "--- Keywords ---\n";

    std::vector<KeywordDefinition> keywords;

    runtime->getKeywords(keywords);

    for (const KeywordDefinition& keyword : keywords) {
        out << keyword.keyword << "\n";
    }

    out << "--- Statements --- \n";

    auto compiler = runtime->createCompiler();
    std::vector<NomadString> statements;
    compiler->getRegisteredStatements(statements);

    for (auto& statement : statements) {
        out << statement << "\n";
    }

    out << "--- Commands ---\n";

    std::vector<CommandDefinition> commands;
    runtime->getCommands(commands);

    for (const CommandDefinition& command : commands) {
        out << command.name << "\n";
    }
}

#endif  // NOMAD_SCRIPT_DOC

}  // nomad