//
// Created by jfbil on 2024-10-07.
//

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/FormatStringCompiler.hpp"

#include "nomad/compiler/Expression.hpp"
#include "nomad/compiler/SyntaxTree.hpp"


namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Expressions
Expression::Expression(NomadIndex line, NomadIndex column, const Type* type):
    AstNode(line, column),
    m_type(type)
{ }

Expression::~Expression() {
    if (m_type != nullptr) {
        m_type->free_value(m_value);
    }
}

void Expression::parse(Compiler* compiler, Script* script) {
    if (!is_parsed()) {
        on_parse(compiler, script);
    }
}

void Expression::compile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (is_parsed()) {
        compiler->add_load_value(m_type, m_value);
    } else {
        on_compile(compiler, script);
    }
}

const Type* Expression::get_type() const {
    return m_type;
}

const ScriptValue& Expression::get_value() const {
    return m_value;
}

bool Expression::has_value() const {
    return m_has_value;
}

bool Expression::has_type() const {
    return m_type != nullptr;
}

bool Expression::is_parsed() const {
    return has_type() && has_value();
}

void Expression::set_resolved(const ScriptValue& value, const Type* type) {
    set_type(type);
    set_value(value);
}

void Expression::set_type(const Type* type) {
    m_type = type;
}

void Expression::set_value(const ScriptValue& value) {
    m_type->copy_value(value, m_value);

    m_has_value = true;
}

///////////////////////////////////////////////////////////////////////////////
// UnaryExpression
UnaryExpression::UnaryExpression(
    NomadIndex line, NomadIndex column, UnaryOperator unary_operator,
    std::unique_ptr<Expression> expression
):
    Expression(line, column),
    m_unary_operator(unary_operator),
    m_expression(std::move(expression)) {
}


void UnaryExpression::on_parse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);

    if (m_expression->is_parsed()) {
        auto expression_type = m_expression->get_type();

        auto value = m_expression->get_value();
        ScriptValue result;

        auto result_type = compiler->get_unary_operator_result_type(m_unary_operator, expression_type);
        auto fold_success = compiler->fold_unary(m_unary_operator, expression_type, value, result);

        if (!fold_success) {
            raise_exception("Cannot fold unary operator");
        }

        set_resolved(result, result_type);
    } else {
        set_type(m_expression->get_type());
    }
}

void UnaryExpression::on_compile(Compiler* compiler, Script* script) {
    on_parse(compiler, script);

    m_expression->compile(compiler, script);

    auto opcode = compiler->get_unary_operator_op_code_id(m_unary_operator, m_expression->get_type());

    compiler->add_op_code(opcode);
}
///////////////////////////////////////////////////////////////////////////////
// BinaryExpression
BinaryExpression::BinaryExpression(NomadIndex line, NomadIndex column, BinaryOperator binary_operator,
    std::unique_ptr<Expression> left, std::unique_ptr<Expression> right
):
    Expression(line, column),
    m_binary_operator(binary_operator),
    m_left(std::move(left)),
    m_right(std::move(right)) {
}

void BinaryExpression::on_parse(Compiler* compiler, Script* script) {
    m_left->parse(compiler, script);
    m_right->parse(compiler, script);

    auto lhs_type = m_left->get_type();
    auto rhs_type = m_right->get_type();

    if (lhs_type != nullptr && rhs_type != nullptr) {

        auto result_type = compiler->get_binary_operator_result_type(m_binary_operator, lhs_type, rhs_type);

        set_type(result_type);
    }

    if (m_left->is_parsed() && m_right->is_parsed()) {
        auto lhs = m_left->get_value();
        auto rhs = m_right->get_value();

        ScriptValue value;

        auto result_type = compiler->get_binary_operator_result_type(m_binary_operator, lhs_type, rhs_type);
        auto result = compiler->fold_binary(m_binary_operator, lhs_type, lhs, rhs_type, rhs, value);

        if (!result) {
            raise_exception("Cannot fold binary operator");
        }

        set_resolved(value, result_type);
    }
}

void BinaryExpression::on_compile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (is_parsed()) {
        compiler->add_load_value(get_type(), get_value());
        return;
    }

    m_right->compile(compiler, script);
    compiler->add_push_result(m_right->get_type());
    m_left->compile(compiler, script);
    compiler->add_pop_intermediate(m_left->get_type());

    auto left_type = m_left->get_type();
    auto right_type = m_right->get_type();

    auto op_code_id = compiler->get_binary_operator_op_code_id(
        m_binary_operator,
        left_type,
        right_type
    );

    if (op_code_id == NOMAD_INVALID_ID) {
        raise_exception("Invalid binary operator");
    }

    compiler->add_op_code(op_code_id);
}

///////////////////////////////////////////////////////////////////////////////
// CallCommandExpression
CallCommandExpression::CallCommandExpression(NomadIndex line, NomadIndex column, NomadString name):
    Expression(line, column),
    m_name(std::move(name)) {
}

void CallCommandExpression::on_parse(Compiler* compiler, Script* script) {
    auto command_id = compiler->get_runtime()->get_command_id(m_name);

    if (command_id == NOMAD_INVALID_ID) {
        raise_exception("Unknown command '" + m_name + "'");
    }

    CommandDefinition command_definition;
    compiler->get_runtime()->get_command_definition(command_id, command_definition);

    set_type(command_definition.return_type);
}

void CallCommandExpression::on_compile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler, script);

    auto command_id = compiler->get_runtime()->get_command_id(m_name);

    if (command_id == NOMAD_INVALID_ID) {
        raise_exception("Unknown command '" + m_name + "'");
    }

    compiler->add_command_call(command_id);
}

void CallCommandExpression::add_argument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* CallCommandExpression::get_arguments() {
    return &m_arguments;
}

///////////////////////////////////////////////////////////////////////////////
// CallScriptExpression
ScriptCallExpression::ScriptCallExpression(NomadIndex line, NomadIndex column, NomadString name):
    Expression(line, column),
    m_name(std::move(name)) {
}

void ScriptCallExpression::on_parse(Compiler* compiler, Script* script) {
    auto call_script_id = compiler->get_runtime()->get_script_id(m_name);

    auto call_script = compiler->get_runtime()->get_script(call_script_id);

    set_type(call_script->get_return_type());
}

void ScriptCallExpression::on_compile(Compiler* compiler, Script* script) {
    auto target_script_id = compiler->get_runtime()->get_script_id(m_name);

    if (target_script_id == NOMAD_INVALID_ID) {
        raise_exception("Unknown script '" + m_name + "'");
    }

    compiler->add_script_call(target_script_id);
}

void ScriptCallExpression::add_argument(std::unique_ptr<Expression> argument) {
    m_arguments.push_back(std::move(argument));
}

NomadIndex ScriptCallExpression::get_argument_count() const {
    return m_arguments.size();
}

///////////////////////////////////////////////////////////////////////////////
// PrimaryExpression
PrimaryExpression::PrimaryExpression(NomadIndex line, NomadIndex column):
    Expression(line, column) {
}

///////////////////////////////////////////////////////////////////////////////
// BooleanLiteral
BooleanLiteral::BooleanLiteral(NomadIndex line, NomadIndex column, NomadBoolean m_value):
    PrimaryExpression(line, column),
    m_value(m_value) {
}

void BooleanLiteral::on_parse(Compiler* compiler, Script* script) {
    set_resolved(ScriptValue{m_value}, compiler->get_runtime()->get_boolean_type());
}

void BooleanLiteral::on_compile(Compiler* compiler, Script* script) {
    compiler->add_load_boolean_value(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// IntegerExpression

IntegerLiteral::IntegerLiteral(NomadIndex line, NomadIndex column, NomadInteger value):
    PrimaryExpression(line, column),
    m_value(value) {
}

void IntegerLiteral::on_parse(Compiler* compiler, Script* script) {
    ScriptValue value{m_value};

    set_resolved(value, compiler->get_runtime()->get_integer_type());
}

void IntegerLiteral::on_compile(Compiler* compiler, Script* script) {
    compiler->add_load_integer_value(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// FloatLiteralExpression
FloatLiteral::FloatLiteral(NomadIndex line, NomadIndex column, NomadFloat value):
    PrimaryExpression(line, column),
    m_value(value) {
}

void FloatLiteral::on_parse(Compiler* compiler, Script* script) {
    ScriptValue value{m_value};

    set_resolved(value, compiler->get_runtime()->get_float_type());
}

void FloatLiteral::on_compile(Compiler* compiler, Script* script) {
    compiler->add_load_float_value(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// StringLiteralExpression

StringLiteral::StringLiteral(NomadIndex line, NomadIndex column, NomadString value):
    PrimaryExpression(line, column),
    m_value(std::move(value)) {
}

void StringLiteral::on_parse(Compiler* compiler, Script* script) {
    set_resolved(ScriptValue{m_value}, compiler->get_runtime()->get_string_type());
}

void StringLiteral::on_compile(Compiler* compiler, Script* script) {
    auto string_value = get_value().get_string_value();

    auto string_id = compiler->get_runtime()->register_string(string_value);

    compiler->add_op_code(OpCodes::op_string_load_r);
    compiler->add_id(string_id);
}

///////////////////////////////////////////////////////////////////////////////
// ConstantValueExpression
ConstantValueExpression::ConstantValueExpression(NomadIndex line, NomadIndex column, const NomadString& constant_name):
    PrimaryExpression(line, column),
    m_constant_name(constant_name) {
}

void ConstantValueExpression::on_parse(Compiler* compiler, Script* script) {
    m_constant_id = compiler->get_runtime()->get_constant_id(m_constant_name);

    if (m_constant_id != NOMAD_INVALID_ID) {
        ScriptValue constant_value;
        compiler->get_runtime()->get_constant_value(m_constant_id, constant_value);
        auto constant_type = compiler->get_runtime()->get_constant_type(m_constant_id);

        set_resolved(constant_value, constant_type);
    }
}

void ConstantValueExpression::on_compile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (!is_parsed()) {
        raise_exception("Unknown constant: " + m_constant_name);
    }

    compiler->add_load_value(get_type(), get_value());
}

///////////////////////////////////////////////////////////////////////////////
// String Literal expressions
///////////////////////////////////////////////////////////////////////////////
//StringLiteralExpression::StringLiteralExpression(NomadIndex line, NomadIndex column, NomadString value):
//    Expression(line, column),
//    m_value(std::move(value))
//{
//}
//
//StringLiteralExpression::~StringLiteralExpression() {
//    get_value().free_string_value();
//}
//
//void StringLiteralExpression::on_parse(Compiler* compiler) {
//    m_string_id = compiler->get_runtime()->get_string_id(m_value);
//
//    if (m_string_id == NOMAD_INVALID_ID) {
//        raise_exception("Unknown string: " + m_value);
//    }
//
//    ScriptValue string_id_value;
//
//    string_id_value.set_id_value(m_string_id);
//
//    set_resolved(string_id_value, compiler->get_runtime()->get_string_type());
//}
//
//void StringLiteralExpression::on_compile(Compiler* compiler) {
//    compiler->add_op_code(OpCodes::op_string_load_r);
//    compiler->add_id(m_string_id);
//}

///////////////////////////////////////////////////////////////////////////////
// FormatStringLiteral
FormatStringLiteral::FormatStringLiteral(NomadIndex line, NomadIndex column, const NomadString& format_string):
    Expression(line, column),
    m_format_string(format_string) {
}

void FormatStringLiteral::on_parse(Compiler* compiler, Script* script) {
    set_type(compiler->get_runtime()->get_string_type());
}

void FormatStringLiteral::on_compile(Compiler* compiler, Script* script) {
    NomadId script_id = script->get_id();

    auto format_string_id = compiler->get_runtime()->get_format_string_id(m_format_string, script_id);

    if (format_string_id == NOMAD_INVALID_ID) {
        format_string_id = compiler->get_runtime()->register_format_string(m_format_string, script_id);

        auto format_string = compiler->get_runtime()->get_format_string(format_string_id);

        compile_format_string(compiler, script, m_format_string, format_string);
    }

    compiler->add_op_code(OpCodes::op_format_string_execute);
    compiler->add_id(format_string_id);
}


IdentifierExpression::IdentifierExpression(NomadIndex line, NomadIndex column, const NomadString& identifier):
    PrimaryExpression(line, column),
    m_identifier(identifier)
{
}
void IdentifierExpression::on_parse(nomad::Compiler* compiler, Script* script) {
    compiler->get_identifier_definition(m_identifier, script, m_identifier_definition);

    // Check for invalid identifier type.
    switch (m_identifier_definition.identifier_type) {
        case IdentifierType::Unknown:
            // Unknown identifier type
            compiler->report_error("Unknown identifier: " + m_identifier);

        case IdentifierType::Script:
            compiler->report_error("Cannot use script as identifier: "+ m_identifier);

        case IdentifierType::Command:
            compiler->report_error("Cannot use command as identifier: "+ m_identifier);

        case IdentifierType::Keyword:
            compiler->report_error("Cannot use keyword as identifier: "+ m_identifier);

        case IdentifierType::Statement:
            compiler->report_error("Cannot use statement as identifier: "+ m_identifier);
    }

    if (m_identifier_definition.identifier_type == IdentifierType::ContextVariable) {
        auto context_id = compiler->get_runtime()->get_variable_context_id_by_prefix(m_identifier);

        if (context_id == NOMAD_INVALID_ID) {
            compiler->report_error("Unknown context for identifier: " + m_identifier);
        }

        auto context = compiler->get_runtime()->get_variable_context(context_id);

        context->register_variable(m_identifier, m_identifier_definition.value_type);
    }

    if (m_identifier_definition.identifier_type == IdentifierType::ScriptVariable) {
        script->register_variable(m_identifier, m_identifier_definition.value_type);
    }

    if (m_identifier_definition.value_type == nullptr) {
        compiler->report_error("Unknown type for identifier: " + m_identifier);
    }

    set_type(m_identifier_definition.value_type);
}

void IdentifierExpression::on_compile(Compiler* compiler, Script* script) {
    switch (m_identifier_definition.identifier_type) {
        case IdentifierType::ScriptVariable:
            compiler->add_op_code(OpCodes::op_script_variable_get);
            compiler->add_id(m_identifier_definition.variable_id);
            break;

        case IdentifierType::DynamicVariable:
            compiler->add_op_code(OpCodes::op_dynamic_variable_get);
            compiler->add_id(m_identifier_definition.variable_id);
            break;

        case IdentifierType::ContextVariable: {
            auto context_id = compiler->get_runtime()->get_variable_context_id_by_prefix(m_identifier);

            if (context_id == NOMAD_INVALID_ID) {
                compiler->report_error("Unknown context for identifier: " + m_identifier);
            }

            auto context = compiler->get_runtime()->get_variable_context(context_id);

            context->register_variable(m_identifier, m_identifier_definition.value_type);

            compiler->add_op_code(OpCodes::op_context_variable_get);
            compiler->add_id(m_identifier_definition.context_id);
            compiler->add_id(m_identifier_definition.variable_id);
            break;
        }

        case IdentifierType::Parameter:
            if (m_identifier_definition.value_type == nullptr) {
                compiler->report_error("Unknown type for parameter: " + m_identifier);
            }

            if (m_identifier_definition.value_type == compiler->get_runtime()->get_string_type()) {
                compiler->add_op_code(OpCodes::op_string_parameter_load_r);
            } else {
                compiler->add_op_code(OpCodes::op_parameter_load_r);
            }
//            compiler->add_op_code(OpCodes::op_parameter_get);
            compiler->add_id(m_identifier_definition.variable_id);
            break;

        default:
            raise_exception("Invalid identifier type");
    }
}

} // namespace nomad