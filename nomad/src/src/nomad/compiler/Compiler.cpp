//
// Created by jfbil on 2023-06-04.
//

#include "nomad/compiler/FormatStringCompiler.hpp"
#include "nomad/compiler/Tokenizer.hpp"
#include "nomad/compiler/StatementParsers.hpp"

#include "nomad/compiler/statements/ReturnStatement.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/system/String.hpp"

#include "nomad/compiler/Compiler.hpp"

#include <filesystem>
#include <fstream>

namespace nomad {

// Utility to concatenate directory / filename
NomadString concat_path(const NomadString& directory, const NomadString& filename) {
    if (filename.empty() && directory.empty()) {
        // I feel like this should be an error...
        return "";
    }

    if (filename.empty()) {
        return directory;
    }

    if (directory.empty()) {
        return filename;
    }

    auto lastChar = directory.back();

    if (lastChar == '/' || lastChar == '\\') {
        return directory + filename;
    }

    return directory + '/' + filename;
}

Compiler::Compiler(Runtime *runtime, std::vector<Instruction>& instructions):
    m_runtime(runtime),
    m_instructions(instructions)
{
    if (m_instructions.empty()) {
        // Add op_stop instruction so that the return of a script is a stop instruction
        add_op_code(OpCodes::op_stop);
    }

    register_parse_statement_fn("const", nullptr, pre_parse_const_statement);
    register_parse_statement_fn("fun", parse_fun_statement, pre_parse_fun_statement);
    register_parse_statement_fn("assert", parse_assert_statement);
//    register_parse_statement_fn("assert_string", parse_string_assert_statement);
    register_parse_statement_fn("if", parse_if_statement);
    register_parse_statement_fn("params", nullptr, pre_parse_params_statement);
    register_parse_statement_fn("return", parse_return_statement);

    auto type_boolean = runtime->get_boolean_type();
    auto type_integer = runtime->get_integer_type();
    auto type_float = runtime->get_float_type();
    auto type_string = runtime->get_string_type();

    register_unary_operator(UnaryOperator::Bang, type_boolean, type_boolean, OpCodes::op_boolean_not, fold_boolean_unary_bang);

    register_unary_operator(UnaryOperator::Plus, type_integer,  type_integer, OpCodes::op_integer_absolute, fold_integer_unary_plus);
    register_unary_operator(UnaryOperator::Minus, type_integer, type_integer, OpCodes::op_integer_negate, fold_integer_unary_minus);

    register_unary_operator(UnaryOperator::Plus, type_float, type_float, OpCodes::op_float_absolute, fold_float_unary_plus);
    register_unary_operator(UnaryOperator::Minus, type_float, type_float, OpCodes::op_float_negate, fold_float_unary_minus);
    register_unary_operator(UnaryOperator::Sin, type_float, type_float, OpCodes::op_float_sin, fold_float_unary_sin);
    register_unary_operator(UnaryOperator::Cos, type_float, type_float, OpCodes::op_float_cosine, fold_float_unary_cos);
    register_unary_operator(UnaryOperator::Tan, type_float, type_float, OpCodes::op_float_tangent, fold_float_unary_tan);

    register_binary_operator(BinaryOperator::EqualEqual, type_boolean, type_boolean, type_boolean, OpCodes::op_boolean_equal, fold_boolean_binary_and_and);
    register_binary_operator(BinaryOperator::BangEqual, type_boolean, type_boolean, type_boolean, OpCodes::op_boolean_not_equal, fold_boolean_binary_pipe_pipe);

    register_binary_operator(BinaryOperator::Plus, type_integer, type_integer, type_integer, OpCodes::op_integer_add, fold_integer_binary_plus);
    register_binary_operator(BinaryOperator::Minus, type_integer, type_integer, type_integer, OpCodes::op_integer_subtract, fold_integer_binary_minus);
    register_binary_operator(BinaryOperator::Star, type_integer, type_integer, type_integer, OpCodes::op_integer_multiply, fold_integer_binary_star);
    register_binary_operator(BinaryOperator::Slash, type_integer, type_integer, type_integer, OpCodes::op_integer_divide, fold_integer_binary_slash);
    register_binary_operator(BinaryOperator::Percent, type_integer, type_integer, type_integer, OpCodes::op_integer_modulo, fold_integer_binary_percent);
    register_binary_operator(BinaryOperator::Caret, type_integer, type_integer, type_integer, OpCodes::op_integer_xor, fold_integer_binary_caret);
    register_binary_operator(BinaryOperator::And, type_integer, type_integer, type_integer, OpCodes::op_integer_and, fold_integer_and);
    register_binary_operator(BinaryOperator::Pipe, type_integer, type_integer, type_integer, OpCodes::op_integer_or, fold_integer_pipe);
    register_binary_operator(BinaryOperator::EqualEqual, type_integer, type_integer, type_boolean, OpCodes::op_integer_equal, fold_integer_binary_equal_equal);
    register_binary_operator(BinaryOperator::BangEqual, type_integer, type_integer, type_boolean, OpCodes::op_integer_not_equal, fold_integer_binary_bang_equal);
    register_binary_operator(BinaryOperator::LessThan, type_integer, type_integer, type_boolean, OpCodes::op_integer_less_than, fold_integer_binary_less_than);
    register_binary_operator(BinaryOperator::LessThanEqual, type_integer, type_integer, type_boolean, OpCodes::op_integer_less_than_or_equal, fold_integer_binary_less_than_equal);
    register_binary_operator(BinaryOperator::GreaterThan, type_integer, type_integer, type_boolean, OpCodes::op_integer_greater_than, fold_integer_binary_greater_than);
    register_binary_operator(BinaryOperator::GreaterThanEqual, type_integer, type_integer, type_boolean, OpCodes::op_integer_greater_than_or_equal, fold_integer_binary_greater_than_equal);

    register_binary_operator(BinaryOperator::Plus, type_float, type_float, type_float, OpCodes::op_float_add, fold_float_binary_plus);
    register_binary_operator(BinaryOperator::Minus, type_float, type_float, type_float, OpCodes::op_float_subtract, fold_float_binary_minus);
    register_binary_operator(BinaryOperator::Star, type_float, type_float, type_float, OpCodes::op_float_multiply, fold_float_binary_star);
    register_binary_operator(BinaryOperator::Slash, type_float, type_float, type_float, OpCodes::op_float_divide, fold_float_binary_slash);
    register_binary_operator(BinaryOperator::EqualEqual, type_float, type_float, type_boolean, OpCodes::op_float_equal, fold_boolean_binary_and_and);
    register_binary_operator(BinaryOperator::BangEqual, type_float, type_float, type_boolean, OpCodes::op_float_not_equal, fold_boolean_binary_pipe_pipe);
    register_binary_operator(BinaryOperator::LessThan, type_float, type_float, type_boolean, OpCodes::op_float_less_than, fold_boolean_binary_pipe_pipe);
    register_binary_operator(BinaryOperator::LessThanEqual, type_float, type_float, type_boolean, OpCodes::op_float_less_than_or_equal, fold_boolean_binary_pipe_pipe);
    register_binary_operator(BinaryOperator::GreaterThan, type_float, type_float, type_boolean, OpCodes::op_float_greater_than, fold_boolean_binary_pipe_pipe);
    register_binary_operator(BinaryOperator::GreaterThanEqual, type_float, type_float, type_boolean, OpCodes::op_float_greater_than_or_equal, fold_boolean_binary_pipe_pipe);
}

Compiler::~Compiler() = default;

Runtime* Compiler::get_runtime() const {
    return m_runtime;
}

void Compiler::report_error(const NomadString& message) {
    throw CompilerException(message);
}

void Compiler::report_error(const NomadString& message, Script* script, Tokenizer* tokenizer) {
    report_error(message, script->get_name(), tokenizer->get_line_index(), tokenizer->get_column_index());
}

void Compiler::report_error(const NomadString& message, const NomadString& script_name, NomadIndex line, NomadIndex column) {
    auto error_message = message + ": '" + script_name + "' at line " + to_string(line) + ", column " + to_string(column);

    report_error(error_message);
}

void Compiler::report_internal_error(const NomadString& message) {
    throw CompilerException("Internal error: " + message);
}
//
//void Compiler::set_current_script(Script* current_script) {
//    m_current_script = current_script;
//}
//
//Script* Compiler::get_current_script() const {
//    return m_current_script;
//}

void Compiler::register_parse_statement_fn(const NomadString& name, ParseStatementFn fn, PreParseStatementFn pre_fn) {
    m_statements.emplace(name, ParseStatementFnRegistration{ pre_fn, fn });
}

bool Compiler::is_statement(const NomadString& name) const {
    return m_statements.find(name) != m_statements.end();
}

bool Compiler::get_parse_statement_fn(const NomadString& name, ParseStatementFn& fn) const {
    auto it = m_statements.find(name);

    if (it != m_statements.end()) {
        fn = it->second.fn;

        return true;
    }

    return false;
}

bool Compiler::get_pre_parse_statement_fn(const NomadString& name, PreParseStatementFn& fn) const {
    auto it = m_statements.find(name);

    if (it != m_statements.end()) {
        fn = it->second.pre_fn;

        return true;
    }

    return false;
}

void Compiler::get_registered_statements(std::vector<NomadString>& parsers) const {
    for (auto& statement: m_statements) {
        parsers.push_back(statement.first);
    }
}

void Compiler::register_unary_operator(
    UnaryOperator op,
    const Type* operand,
    const Type* result,
    const NomadString& op_code_name,
    UnaryFoldingFn fn
) {
    // Sanity check
    if (m_runtime->get_instruction_id(op_code_name) == NOMAD_INVALID_ID) {
        report_internal_error("[Compiler::register_unary_operator]: Unknown op code '" + op_code_name + "'");
    }

    // Make sure the operator with the same and type doesn't already exist
    for (auto& definition: m_unary_operators) {
        if (definition.op == op && definition.operand == operand) {
            report_internal_error("[Compiler::register_unary_operator]: Unary operator '" + op_code_name + "' already registered for type '" + operand->get_name() + "'");
        }
    }

    auto op_code_id = m_runtime->get_instruction_id(op_code_name);

    // Sanity check
    if (op_code_id == NOMAD_INVALID_ID) {
        report_internal_error("[Compiler::register_unary_operator]: Unknown op code '" + op_code_name + "'");
    }

    m_unary_operators.emplace_back(UnaryOperatorRegistration{op, operand, result, op_code_id, fn});
}

void Compiler::register_binary_operator(
    BinaryOperator op,
    const Type* lhs,
    const Type* rhs,
    const Type* result,
    const NomadString& op_code_name,
    BinaryFoldingFn fn
) {
    // Sanity check
    if (m_runtime->get_instruction_id(op_code_name) == NOMAD_INVALID_ID) {
        report_internal_error("[Compiler::register_binary_operator]: Unknown op code '" + op_code_name + "'");
    }

    // Make sure the operator with the same and type doesn't already exist
    for (auto& definition: m_binary_operators) {
        if (definition.op == op && definition.lhs == lhs && definition.rhs == rhs) {
            report_internal_error("[Compiler::register_binary_operator]: Binary operator '" + op_code_name + "' already registered for types '" + lhs->get_name() + "' and '" + rhs->get_name() + "'");
        }
    }

    auto op_code_id = m_runtime->get_instruction_id(op_code_name);

    // Sanity check
    if (op_code_id == NOMAD_INVALID_ID) {
        report_internal_error("[Compiler::register_binary_operator]: Unknown op code '" + op_code_name + "'");
    }

    m_binary_operators.emplace_back(BinaryOperatorRegistration{op, lhs, rhs, result, op_code_id, fn});
}

const Type* Compiler::get_unary_operator_result_type(UnaryOperator op, const Type* operand_type) const {
    for (auto& definition: m_unary_operators) {
        if (definition.op == op && definition.operand == operand_type) {
            return definition.result;
        }
    }

    return nullptr;
}

const Type* Compiler::get_binary_operator_result_type(BinaryOperator op, const Type* lhs_type, const Type* rhs_type) const {
    for (auto& definition: m_binary_operators) {
        if (definition.op == op && definition.lhs == lhs_type && definition.rhs == rhs_type) {
            return definition.result;
        }
    }

    return nullptr;
}

NomadId Compiler::get_unary_operator_op_code_id(UnaryOperator op, const Type* operand) const {
    for (auto& definition: m_unary_operators) {
        if (definition.op == op && definition.operand == operand) {
            return definition.op_code_id;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Compiler::get_binary_operator_op_code_id(BinaryOperator op, const Type* lhs, const Type* rhs) const {
    for (auto& definition: m_binary_operators) {
        if (definition.op == op && definition.lhs == lhs && definition.rhs == rhs) {
            return definition.op_code_id;
        }
    }

    return NOMAD_INVALID_ID;
}

bool Compiler::fold_unary(
    UnaryOperator op,
    const Type* operand_type,
    const ScriptValue& value,
    ScriptValue& result
) const {
    for (auto& fn: m_unary_operators) {
        if (fn.op == op && fn.operand == operand_type) {
            fn.fn(value, result);

            return true;
        }
    }

    return false;
}

bool Compiler::fold_binary(
    BinaryOperator op,
    const Type* lhs_type,
    const ScriptValue& lhs,
    const Type* rhs_type,
    const ScriptValue& rhs,
    ScriptValue& result
) const {
    for (auto& fn: m_binary_operators) {
        if (fn.op == op && fn.lhs == lhs_type && fn.rhs == rhs_type) {
            fn.fn(lhs, rhs, result);

            return true;
        }
    }

    return false;
}

IdentifierType Compiler::get_identifier_type(const NomadString& name, Script* script) {
    if (m_runtime->get_keyword_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Keyword;
    }

    if (is_statement(name)) {
        return IdentifierType::Statement;
    }

    CommandDefinition commandDefinition;

    if (m_runtime->get_command_definition(name, commandDefinition)) {
        return IdentifierType::Command;
    }

    if (m_runtime->get_constant_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Constant;
    }

    if (m_runtime->get_script_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Script;
    }

    if (m_runtime->get_dynamic_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::DynamicVariable;
    }

    if (m_runtime->get_variable_context_id_by_prefix(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ContextVariable;
    }

    if (script->get_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ScriptVariable;
    }

    return IdentifierType::Unknown;
}

void Compiler::get_identifier_definition(const NomadString& name, Script* script, IdentifierDefinition& definition) {
    if (name.empty()) {
        definition.identifier_type = IdentifierType::Unknown;

        return;
    }

    char first_character = name[0];
    if (!(std::isalnum(first_character) || first_character != '_')) {
        definition.identifier_type = IdentifierType::Unknown;

        return;
    }

    definition.identifier_type = get_identifier_type(name, script);

    NomadId command_id = m_runtime->get_command_id(name);

    if (command_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Command;

        CommandDefinition command_definition;

        m_runtime->get_command_definition(command_id, command_definition);

        definition.command_id = command_id;
        definition.value_type = command_definition.return_type;

        return;
    }

    NomadId script_id = m_runtime->get_script_id(name);

    if (script_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Script;
        definition.variable_id = script_id;
        definition.value_type = m_runtime->get_script(script_id)->get_return_type();

        return;
    }

    NomadId constant_id = m_runtime->get_constant_id(name);

    if (constant_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Constant;
        definition.variable_id = constant_id;
        definition.value_type = m_runtime->get_constant_type(constant_id);

        return;
    }

    NomadId dynamic_variable_id = m_runtime->get_dynamic_variable_id(name);

    if (dynamic_variable_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::DynamicVariable;
        definition.variable_id = dynamic_variable_id;
        definition.value_type = m_runtime->get_dynamic_variable_type(dynamic_variable_id);

        return;
    }

    NomadId context_id = m_runtime->get_variable_context_id_by_prefix(name);

    if (context_id != NOMAD_INVALID_ID) {
        auto variable_id = m_runtime->get_context_variable_id(context_id, name);
        definition.context_id = context_id;
        definition.variable_id = variable_id;
        definition.value_type = m_runtime->get_context_variable_type(context_id, variable_id);

        return;
    }

    NomadId variable_id = script->get_variable_id(name);

    if (variable_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::ScriptVariable;
        definition.variable_id = variable_id;
        definition.value_type = script->get_variable_type(variable_id);

        return;
    }

    NomadId parameter_id = script->get_parameter_id(name);

    if (parameter_id != NOMAD_INVALID_ID) {
        definition.identifier_type = IdentifierType::Parameter;
        definition.variable_id = parameter_id;
        definition.value_type = script->get_parameter_type(parameter_id);

        return;
    }

    definition.identifier_type = IdentifierType::Unknown;
}


NomadIndex Compiler::get_op_code_size() const {
    return m_instructions.size();
}

NomadIndex Compiler::add_op_code(NomadId op_code) {
    auto fn = m_runtime->get_instruction_fn(op_code);

    if (fn == nullptr) {
        report_internal_error("[Compiler::add_op_code] Unknown op code: " + to_string(op_code));
    }

    m_instructions.emplace_back(fn);

    return m_instructions.size() - 1;
}

NomadIndex Compiler::add_op_code(const NomadString& op_code_name) {
    auto op_code_id = m_runtime->get_instruction_id(op_code_name);

    if (op_code_id == NOMAD_INVALID_ID) {
        report_error("[Compiler::add_op_code] Unknown instruction: " + op_code_name);
    }

    return add_op_code(op_code_id);
}

NomadIndex Compiler::add_id(NomadId id) {
    auto index = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(id));

    return index;
}

NomadIndex Compiler::add_index(NomadIndex index) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(index));

    return instruction;
}

NomadIndex Compiler::add_integer(NomadInteger value) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(value));

    return instruction;
}

NomadIndex Compiler::add_float(NomadFloat value) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(value));

    return instruction;
}

NomadIndex Compiler::add_load_value(const Type* type, const ScriptValue& value) {
    if (type == get_runtime()->get_boolean_type()) {
        return add_load_boolean_value(value.get_boolean_value());
    } else if (type == get_runtime()->get_integer_type()) {
        return add_load_integer_value(value.get_integer_value());
    } else if (type == get_runtime()->get_float_type()) {
        return add_load_float_value(value.get_float_value());
    } else if (type == get_runtime()->get_string_type()) {
        return add_load_string_value(value.get_string_value());
    }

    report_internal_error("[ScriptBuilder::add_load_value] Unknown type: " + type->get_name());

    return NOMAD_INVALID_INDEX;
}

NomadIndex Compiler::add_load_boolean_value(bool value) {
    if (value) {
        return add_op_code(OpCodes::op_boolean_load_true_r);
    } else {
        return add_op_code(OpCodes::op_boolean_load_false_r);
    }
}

NomadIndex Compiler::add_load_integer_value(NomadInteger value) {
    if (value == 0) {
        return add_op_code(OpCodes::op_integer_load_zero_r);
    } else if (value == 1) {
        return add_op_code(OpCodes::op_integer_load_one_r);
    } else {
        add_op_code(OpCodes::op_integer_load_r);

        return add_integer(value);
    }
}

NomadIndex Compiler::add_load_float_value(NomadFloat value) {
    if (value == 0) {
        return add_op_code(OpCodes::op_float_load_zero_r);
    } else if (value == 1) {
        return add_op_code(OpCodes::op_float_load_one_r);
    } else {
        add_op_code(OpCodes::op_float_load_r);
        return add_float(value);
    }
}

NomadIndex Compiler::add_load_string_value(const NomadString& value) {
    auto string_id = get_runtime()->register_string(value);

    add_op_code(OpCodes::op_string_load_r);

    return add_id(string_id);
}

NomadIndex Compiler::add_push_result(const Type* type) {
    if (type == get_runtime()->get_string_type()) {
        return add_op_code(OpCodes::op_string_push_r);
    }

    return add_op_code(OpCodes::op_push_r);
}

NomadIndex Compiler::add_pop_intermediate(const Type* type) {
    if (type == get_runtime()->get_string_type()) {
        return add_op_code(OpCodes::op_string_pop_i);
    }

    return add_op_code(OpCodes::op_pop_i);
}

NomadIndex Compiler::add_script_call(NomadId target_script_id) {
    auto script = get_runtime()->get_script(target_script_id);

    if (script == nullptr) {
        report_internal_error("[ScriptBuilder::add_script_call] Unknown script id: " + to_string(target_script_id));
    }
    
    add_op_code(OpCodes::op_call_script);

    if (script->get_script_start() == NOMAD_INVALID_INDEX) {
        add_script_link(target_script_id, m_instructions.size());

        add_index(NOMAD_INVALID_ID); // Placeholder for script jump index
    } else {
        add_index(script->get_script_start());
    }

//    // Free string parameters
//    if (script->get_parameter_count() != 0) {
//        for (auto i = 0; i < script->get_parameter_count(); i++) {
//            if (script->get_parameter_type(i) == get_runtime()->get_string_type()) {
//                add_op_code(OpCodes::op_string_free_stack);
//                add_index(i);
//            }
//        }
//    }

    NomadIndex parameter_count = script->get_parameter_count();

    if (parameter_count != 0) {
        for (auto i = 0; i < parameter_count; i++) {
            auto parameter_type = script->get_parameter_type(i);

            if (parameter_type == get_runtime()->get_string_type()) {
                add_op_code(OpCodes::op_string_free_stack);
                add_index(i);
            }
        }

        add_op_code(OpCodes::op_pop_n);
        add_index(parameter_count);
    }

    return m_instructions.size() - 1;
}

NomadIndex Compiler::add_command_call(NomadId command_id) {
    CommandDefinition command;

    auto result = m_runtime->get_command_definition(command_id, command);

    if (result == false) {
        report_internal_error("[ScriptBuilder::add_command_call] Unknown command: " + to_string(command_id));
    }

    add_op_code(OpCodes::op_call_command);
    add_index(command_id);

    if (!command.parameters.empty()) {
        for (auto i = 0; i < command.parameters.size(); i++) {
            auto& parameter = command.parameters[i];

            if (parameter.type == get_runtime()->get_string_type()) {
                add_op_code(OpCodes::op_string_free_stack);
                add_index(i);
            }
        }

        add_op_code(OpCodes::op_pop_n);
        add_index(command.parameters.size());
    }

    return m_instructions.size() - 1;
}

void Compiler::set_op_code(NomadIndex index, const NomadString& op_code_name) {
    NomadId op_code_id = m_runtime->get_instruction_id(op_code_name);

    if (op_code_id == NOMAD_INVALID_ID) {
        report_internal_error("[ScriptBuilder::set_op_code] Unknown op code: " + op_code_name);
    }

    set_op_code(index, op_code_id);
}

void Compiler::set_op_code(NomadIndex index, NomadId op_code_id) {
    auto fn = m_runtime->get_instruction_fn(op_code_id);

    if (fn == nullptr) {
        report_internal_error("[ScriptBuilder::set_op_code] Unknown op code: " + to_string(op_code_id));
    }

    m_instructions[index].fn = fn;
}


void Compiler::set_id(size_t index, NomadId id) {
    m_instructions[index].value.set_id_value(id);
}

void Compiler::set_index(NomadIndex index, NomadIndex value) {
    m_instructions[index].value.set_index_value(value);
}

void Compiler::pre_parse_script(ScriptFile& script_file) {
    auto& tokens = script_file.tokens;

    tokens.reset();

    while (tokens.next_line()) {
        auto& statement = tokens.next_token();

        PreParseStatementFn pre_parse_statement_fn;

        auto script = m_runtime->get_script(script_file.script_id);

        if (get_pre_parse_statement_fn(statement.text_value, pre_parse_statement_fn) && pre_parse_statement_fn) {
            pre_parse_statement_fn(this, script, &tokens);
        }
    }
}


void Compiler::parse_script(ScriptFile& file) {
//    auto script_id = m_runtime->register_script(script_file.script_name, script_file.file_name, script_file.source);

//    auto script_id = register_script_source(script_file.script_name, script_file.file_name, script_file.source);
//
//    auto script = m_runtime->get_script(script_id);

    auto ast = std::make_unique<ScriptNode>(0, 0);
    auto& tokens = file.tokens;

    tokens.reset();

    while (tokens.next_line()) {
        auto script = m_runtime->get_script(file.script_id);

        auto statement = parser::parse_line(this, script, &tokens);

        if (statement) {
            ast->add_statement(std::move(statement));
        }
    }

    set_script_node(file.script_id, std::move(ast));
}

void Compiler::compile_script(ScriptSource& source) {
    Script* script = source.script;

    script->set_script_start(m_instructions.size());

    // Initialize string variables.
    NomadIndex variable_count = script->get_variable_count();

    if (variable_count != 0) {
        add_op_code(OpCodes::op_push_n);
        add_index(variable_count);
        for (auto i = 0; i < variable_count; i++) {
            if (script->get_variable_type(i) == get_runtime()->get_string_type()) {
                add_op_code(OpCodes::op_string_init);
                add_index(i);
            }
        }
    }

    source.ast->compile(this, script);

    // Free string variables.
    if (variable_count != 0) {
        for (auto i = 0; i < variable_count; i++) {
            if (script->get_variable_type(i) == get_runtime()->get_string_type()) {
                add_op_code(OpCodes::op_string_free_variable);
                add_index(i);
            }

        }
        add_op_code(OpCodes::op_return_n);
        add_index(variable_count);
    } else {
        add_op_code(OpCodes::op_return);
    }


    script->set_script_end(m_instructions.size());
}

NomadId Compiler::register_script_file(
    const NomadString& script_name,
    const NomadString& file_name,
    const NomadString& source
) {
    auto script_file = ScriptFile {
        script_name,
        file_name,
        source,
        {m_runtime, source},
    };

    // Also register script so it is available for linking
    script_file.script_id = register_script_source(script_name, file_name, source);

    m_files.emplace_back(std::move(script_file));

    // Sort scripts alphabetically to ensure consistent order in compiling across systems.
    std::sort(m_files.begin(), m_files.end(), [](const ScriptFile& a, const ScriptFile& b) {
        return a.script_name < b.script_name;
    });

    return script_file.script_id;
}

NomadId Compiler::register_script_source(
    const NomadString& script_name,
    const NomadString& file_name,
    const NomadString& source
) {
    auto script_id = m_runtime->register_script(script_name, file_name, source);

    if (script_id == NOMAD_INVALID_ID) {
        report_error("Could not register script '" + script_name + "' (" + file_name + ")");
    }

    auto script = m_runtime->get_script(script_id);

    m_sources.emplace_back(ScriptSource {
        script_name,
        {},
        script
    });

    return script_id;
}

void Compiler::set_script_node(NomadId script_id, std::unique_ptr<ScriptNode> ast) {
    for (auto& source: m_sources) {
        if (source.script->get_id() == script_id) {
            source.ast = std::move(ast);

            return;
        }
    }

    report_internal_error("[Compiler::set_script_node] Unknown script id: " + to_string(script_id));
}

void Compiler::load_scripts_from_path(const NomadString& path) {
    scan_directory_for_scripts(path, "", 10);
}

void Compiler::compile_scripts() {
    log::info("Pre-Parsing scripts");

    for (auto& script_file: m_files) {
        try {
            log::debug("Pre-Parsing '" + script_file.script_name + "' (" + script_file.file_name + ")");

            pre_parse_script(script_file);
        } catch (std::exception& e) {
            report_error("Failed to pre-parse script '" + script_file.script_name + "' (" + script_file.file_name + "): " + e.what());
        }
    }

    log::info("Parsing scripts");

    for (auto& script_file: m_files) {
        try {
            log::debug("Parsing '" + script_file.script_name + "' (" + script_file.file_name + ")");

            parse_script(script_file);
        } catch (std::exception& e) {
            report_error("Failed to parse script '" + script_file.script_name + "' (" + script_file.file_name + "): " + e.what());
        }
    }

    log::info("Syntax checking scripts");

    for (auto& source: m_sources) {
        try {
            log::debug("Syntax checking script '" + source.script->get_name() + "' (" + source.script->get_path() + ")");

            source.ast->parse(this, source.script);
        } catch (std::exception& e) {
            report_error("Failed to syntax check script '" + source.script->get_name() + "' (" + source.script->get_path() + "): " + e.what());
        }
    }

    log::info("Compiling scripts");

    for (auto& source: m_sources) {
        try {
            log::debug("Compiling script '" + source.script->get_name() + "' (" + source.script->get_path() + ")");

            compile_script(source);
        } catch (std::exception& e) {
            report_error("Failed to compile script '" + source.script->get_name() + "' (" + source.script->get_path() + "): " + e.what());
        }
    }

    log::info("Linking scripts");

    link_scripts();

    m_script_links.clear();

    log::info("Scripts compiled");
}

void Compiler::add_script_link(NomadId script_id, NomadIndex call_index) {
    m_script_links.emplace_back(ScriptLink{script_id, call_index});
}

void Compiler::link_scripts() {
    for (auto& link: m_script_links) {
        auto script = m_runtime->get_script(link.script_id);

        if (script == nullptr) {
            report_error("[Compiler::link_scripts] Failed to link script '" + to_string(link.script_id) + "'");
        }
        if (script->get_script_start() == NOMAD_INVALID_INDEX) {
            report_error("[Compiler::link_scripts] Script '" + script->get_name() + "' has not been compiled");
        }

        set_index(link.call_index, script->get_script_start());
    }
}

void Compiler::scan_directory_for_scripts(const NomadString& base_path, const NomadString& sub_path, NomadIndex max_depth) {
    if (max_depth == 0) {
        log::debug("Skipping directory '" + sub_path + "' (max depth reached)");

        return;
    }

    const NomadString extension = ".nomad";

    auto path_string = concat_path(base_path, sub_path);
    std::filesystem::path path{path_string};

    for (auto& entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            auto directory_name = NomadString(entry.path().filename().string());
            auto new_sub_path = concat_path(sub_path, directory_name);
            scan_directory_for_scripts(base_path, new_sub_path, max_depth - 1);
        } else if (entry.is_regular_file() && entry.path().extension() == extension) {
            auto file_name = base_path + "/" + sub_path + "/" + NomadString(entry.path().filename().string());

            auto start = base_path.length() + 1;
            auto length = entry.path().string().length() - start - extension.length();

            auto script_name = NomadString(entry.path().string().substr(start, length));
            std::replace(script_name.begin(), script_name.end(), '\\', '.');
            std::replace(script_name.begin(), script_name.end(), '/', '.');

            // Read source
            std::ifstream file(file_name.c_str(), std::ios::in);
            if (!file.is_open()) {
                report_error("Failed to open file '" + file_name + "'");
            }

            NomadString source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

            register_script_file(script_name, file_name, source);
       }
    }
}

NomadString Compiler::generate_script_name(const NomadString& generated_name, const NomadString& host_script_name, NomadIndex line) {
    NomadString generated_script_name;
    auto index = 0;

    do {
        generated_script_name = SCRIPT_INTERNAL_NAME_PREFIX + generated_name + "_" + host_script_name + "_" + std::to_string(line);
    } while (m_runtime->get_script_id(generated_script_name) != NOMAD_INVALID_ID);

    return generated_script_name;
}

NomadString Compiler::generate_script_name(const NomadString& name, const Script* script, NomadIndex line) {
    return generate_script_name(name, script->get_name(), line);
}

} // nomad