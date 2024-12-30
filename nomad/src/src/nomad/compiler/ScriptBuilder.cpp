//
// Created by jfbil on 2023-06-04.
//

#include "nomad/compiler/ScriptBuilder.hpp"

#include "nomad/system/String.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/Nomad.hpp"

namespace nomad {

ScriptBuilder::ScriptBuilder(Script* script, Compiler* compiler) :
    m_script(script),
    m_compiler(compiler) {
}

ScriptBuilder::~ScriptBuilder() = default;

void ScriptBuilder::report_error(const NomadString& message, Tokenizer* parser) {
    report_error(message, parser->get_line_index(), parser->get_column_index());
}

void ScriptBuilder::report_error(const NomadString& message, NomadIndex line, NomadIndex column) {
    NomadString error_message = m_script->get_name() + "[" + to_string(line) + ":" + to_string(column) + "] " + message;

    throw ScriptBuilderException(error_message);
}

Script* ScriptBuilder::get_script() const {
    return m_script;
}

Compiler* ScriptBuilder::get_compiler() const {
    return m_compiler;
}

Runtime* ScriptBuilder::get_runtime() const {
    return m_compiler->get_runtime();
}

void ScriptBuilder::add_parameter(const NomadString& parameter_name, Type* type) {
    m_script->add_parameter(parameter_name, type);
}

NomadId ScriptBuilder::get_parameter_id(const NomadString& parameter_name) const {
    return m_script->get_parameter_id(parameter_name);
}

const NomadString& ScriptBuilder::get_parameter_name(NomadId parameter_id) const {
    return m_script->get_parameter_name(parameter_id);
}

const Type* ScriptBuilder::get_parameter_type(NomadId parameter_id) const {
    return m_script->get_parameter_type(parameter_id);
}

NomadIndex ScriptBuilder::get_parameter_count() const {
    return m_script->get_parameter_count();
}

IdentifierType ScriptBuilder::get_identifier_type(const NomadString& name) {
    if (m_compiler->get_runtime()->get_keyword_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Keyword;
    }

    if (m_compiler->is_statement(name)) {
        return IdentifierType::Statement;
    }

    CommandDefinition commandDefinition;

    if (m_compiler->get_runtime()->get_command_definition(name, commandDefinition)) {
        return IdentifierType::Command;
    }

    if (m_compiler->get_runtime()->get_constant_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Constant;
    }

    if (m_compiler->get_runtime()->get_script_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Script;
    }

    if (m_compiler->get_runtime()->get_dynamic_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::DynamicVariable;
    }

    if (m_compiler->get_runtime()->get_variable_context_id_by_prefix(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ContextVariable;
    }

    if (get_script_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ScriptVariable;
    }

    return IdentifierType::Unknown;
}

void ScriptBuilder::get_identifier_definition(const NomadString& name, IdentifierDefinition& definition) {
    if (name.empty()) {
        definition.identifier_type = IdentifierType::Unknown;

        return;
    }

    char first_character = name[0];
    if (!(std::isalnum(first_character) || first_character != '_')) {
        definition.identifier_type = IdentifierType::Unknown;

        return;
    }

    definition.identifier_type = get_identifier_type(name);

    NomadId command_id = m_compiler->get_runtime()->get_command_id(name);

    if (command_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Command;

        CommandDefinition command_definition;

        m_compiler->get_runtime()->get_command_definition(command_id, command_definition);

        definition.command_id = command_id;
        definition.value_type = command_definition.return_type;

        return;
    }

    NomadId script_id = m_compiler->get_runtime()->get_script_id(name);

    if (script_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Script;
        definition.variable_id = script_id;

        return;
    }

    NomadId constant_id = m_compiler->get_runtime()->get_constant_id(name);

    if (constant_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Constant;
        definition.variable_id = constant_id;

        return;
    }

    NomadId dynamic_variable_id = m_compiler->get_runtime()->get_dynamic_variable_id(name);

    if (dynamic_variable_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::DynamicVariable;
        definition.variable_id = dynamic_variable_id;

        return;
    }

    NomadId context_id = m_compiler->get_runtime()->get_variable_context_id_by_prefix(name);

    if (context_id != NOMAD_INVALID_ID) {
        auto variable_id = m_compiler->get_runtime()->get_context_variable_id(context_id, name);
        definition.context_id = context_id;
        definition.variable_id = variable_id;

        return;
    }

    NomadId variable_id = get_script_variable_id(name);

    if (variable_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::ScriptVariable;
        definition.variable_id = variable_id;

        return;
    }

    definition.identifier_type = IdentifierType::Unknown;
}

NomadId ScriptBuilder::register_script_variable(const NomadString& name, Type* type) {
    return m_script->register_variable(name, type);
}

NomadId ScriptBuilder::get_script_variable_id(const NomadString& name) {
    return m_script->get_variable_id(name);
}

NomadId ScriptBuilder::register_format_string(const FormatString& format_string) {
    return m_script->register_format_string(format_string);
}

NomadId ScriptBuilder::get_format_string_id(const NomadString& format_string) const {
    return m_script->get_format_string_id(format_string);
}

NomadIndex ScriptBuilder::get_op_code_size() const {
    return m_op_codes.size();
}

NomadIndex ScriptBuilder::add_op_code(NomadId op_code) {
    m_op_codes.push_back(op_code);

    return m_op_codes.size() - 1;
}

NomadIndex ScriptBuilder::add_op_code(const NomadString& op_code_name) {
    auto op_code_id = m_compiler->get_op_code_id(op_code_name);

    if (op_code_id == NOMAD_INVALID_ID) {
        report_internal_error("Unknown instruction: " + op_code_name);
    }

    m_op_codes.push_back(op_code_id);

    return m_op_codes.size() - 1;
}
//
//NomadIndex ScriptBuilder::add_op_code(const NomadString& operator_name) {
//    auto operator_id = m_compiler->get_runtime()->get_operator_id(operator_name);
//
//    if (operator_id == NOMAD_INVALID_ID) {
//        report_internal_error("Unknown operator: " + operator_name);
//    }
//
//    m_op_codes.push_back(operator_id);
//
//    return m_op_codes.size() - 1;
//}
//
//NomadIndex ScriptBuilder::add_string_operator(const nomad::NomadString& string_operator_name) {
//    auto string_operator_id = m_compiler->get_runtime()->get_string_operator_id(string_operator_name);
//
//    if (string_operator_id == NOMAD_INVALID_ID) {
//        report_internal_error("Unknown operator: " + string_operator_name);
//    }
//
//    m_op_codes.push_back(string_operator_id);
//
//    return string_operator_id;
//}

NomadIndex ScriptBuilder::add_id(NomadId id) {
    auto index = m_op_codes.size();

    m_op_codes.resize(index + sizeof(NomadId) / sizeof(NomadId));

    reinterpret_cast<NomadId*>(&m_op_codes[index])[0] = id;

    return index;
}

NomadIndex ScriptBuilder::add_index(NomadIndex index) {
    auto index = m_op_codes.size();

    m_op_codes.resize(index + sizeof(NomadIndex) / sizeof(NomadId));

    reinterpret_cast<NomadIndex*>(&m_op_codes[index])[0] = index;

    return index;
}

NomadIndex ScriptBuilder::add_short(NomadShort value) {
    auto index = m_op_codes.size();

    m_op_codes.push_back(value);

    return index;
}

NomadIndex ScriptBuilder::add_integer(NomadInteger value) {
    auto index = m_op_codes.size();

    m_op_codes.resize(index + sizeof(NomadInteger) / sizeof(NomadId));

    reinterpret_cast<NomadInteger*>(&m_op_codes[index])[0] = value;

    return index;
}

NomadIndex ScriptBuilder::add_float(NomadFloat value) {
    auto index = m_op_codes.size();

    m_op_codes.resize(index + sizeof(NomadFloat) / sizeof(NomadId));

    reinterpret_cast<NomadFloat*>(&m_op_codes[index])[0] = value;

    return index;
}

NomadIndex ScriptBuilder::add_load_value(const Type* type, const ScriptValue& value) {
    if (type == m_compiler->get_runtime()->get_boolean_type()) {
        return add_load_boolean_value(value.get_boolean_value());
    } else if (type == m_compiler->get_runtime()->get_integer_type()) {
        return add_load_integer_value(value.get_integer_value());
    } else if (type == m_compiler->get_runtime()->get_float_type()) {
        return add_load_float_value(value.get_float_value());
    } else if (type == m_compiler->get_runtime()->get_string_type()) {
        return add_load_string_value(value.get_string_value());
    }

    report_internal_error("[ScriptBuilder::add_load_value] Unknown type: " + type->get_name());
}

NomadIndex ScriptBuilder::add_load_boolean_value(bool value) {
    if (value) {
        return add_op_code(OpCodes::op_boolean_load_true_r);
    } else {
        return add_op_code(OpCodes::op_boolean_load_false_r);
    }
}

NomadIndex ScriptBuilder::add_load_integer_value(NomadInteger value) {
    if (value == 0) {
        return add_op_code(OpCodes::op_integer_load_zero_r);
    } else if (value == 1) {
        return add_op_code(OpCodes::op_integer_load_one_r);
    } else {
        add_op_code(OpCodes::op_integer_load_r);

        return add_integer(value);
    }
}

NomadIndex ScriptBuilder::add_load_float_value(NomadFloat value) {
    if (value == 0) {
        return add_op_code(OpCodes::op_float_load_zero_r);
    } else if (value == 1) {
        return add_op_code(OpCodes::op_float_load_one_r);
    } else {
        add_op_code(OpCodes::op_float_load_r);
        return add_float(value);
    }
}

NomadIndex ScriptBuilder::add_load_string_value(const NomadString& value) {
    auto string_id = m_compiler->get_runtime()->get_string_id(value);

    if (string_id == NOMAD_INVALID_ID) {
        string_id = m_compiler->get_runtime()->register_string(value);
    }

    add_op_code(OpCodes::op_string_load_r);
    return add_id(string_id);
}

NomadIndex ScriptBuilder::add_push_result(const Type* type) {
    if (type == m_compiler->get_runtime()->get_string_type()) {
        return add_op_code(OpCodes::op_string_push_r);
    }

    return add_op_code(OpCodes::op_push_r);
}

NomadIndex ScriptBuilder::add_pop_intermediate(const Type* type) {
    if (type == m_compiler->get_runtime()->get_string_type()) {
        return add_op_code(OpCodes::op_string_pop_i);
    }

    return add_op_code(OpCodes::op_pop_i);
}

NomadIndex ScriptBuilder::add_script_call(NomadId script_id) {
    auto script = m_compiler->get_runtime()->get_script(script_id);

    if (script == nullptr) {
        report_internal_error("[ScriptBuilder::add_script_call] Unknown script: " + to_string(script_id));
    }

    add_op_code(OpCodes::op_call_script);
    return add_index(script->get_script_start());
}

void ScriptBuilder::set_op_code(NomadIndex index, const NomadString& op_code_name) {
    NomadId op_code_id = m_compiler->get_op_code_id(op_code_name);

    if (op_code_id == NOMAD_INVALID_ID) {
        report_internal_error("[ScriptBuilder::set_op_code] Unknown op code: " + op_code_name);
        return;
    }

    set_op_code(index, op_code_id);
}

void ScriptBuilder::set_op_code(NomadIndex index, NomadId op_code_id) {
    m_op_codes[index] = op_code_id;
}


void ScriptBuilder::set_id(size_t index, NomadId id) {
    m_op_codes[index] = id;
}

void ScriptBuilder::set_short(size_t index, NomadShort value) {
    m_op_codes[index] = value;
}

void ScriptBuilder::insert_instruction(size_t index, const NomadString& instruction_name) {
    auto instruction = m_compiler->get_op_code_id(instruction_name);

    if (instruction == NOMAD_INVALID_ID) {
        report_internal_error("Unknown instruction: " + instruction_name);
        return;
    }

    m_op_codes.insert(m_op_codes.begin() + int(index), instruction);
}

void ScriptBuilder::insert_id(size_t index, NomadId id) {
    m_op_codes.insert(m_op_codes.begin() + int(index), id);
}

void ScriptBuilder::insert_short(size_t index, NomadShort value) {
    m_op_codes.insert(m_op_codes.begin() + int(index), value);
}

NomadId ScriptBuilder::get_op_code_id(size_t index) const {
    return m_op_codes[index];
}

void ScriptBuilder::end_script() {
    add_instruction("i_return_zero");

    m_script->set_code(m_op_codes);
}

void ScriptBuilder::report_internal_error(const NomadString& message) {
    auto error_message = "Internal error: " + message;

    throw ScriptBuilderException(error_message);
}

} // nomad