//
// Created by jfbil on 2024-10-11.
//

#include "nomad/compiler/Argument.hpp"

#include "nomad/compiler/Compiler.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Argument
///////////////////////////////////////////////////////////////////////////////
Argument::Argument(NomadIndex line, NomadIndex column, const Type* type):
    AstNode(line, column),
    m_type(type)
{
}

const Type* Argument::get_type() const {
    return m_type;
}

void Argument::generate_code(Compiler* compiler, Script* script) {
    on_compile(compiler, script);
}

///////////////////////////////////////////////////////////////////////////////
// ExpressionArgument
///////////////////////////////////////////////////////////////////////////////
ExpressionArgument::ExpressionArgument(
    NomadIndex line,
    NomadIndex column,
    const Type* type,
    std::unique_ptr<Expression> expression
):
    Argument(line, column, type),
    m_expression(std::move(expression))
{
}

void ExpressionArgument::on_compile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    auto argument_type = get_type();
    auto expression_type = m_expression->get_type();

    if (!argument_type->same_type(expression_type)) {
        compiler->report_error("Argument type mismatch. Expected " + argument_type->get_name() + " but got " + expression_type->get_name());
        return;
    }

    if (expression_type == compiler->get_runtime()->get_string_type()) {
        compiler->add_op_code(OpCodes::op_string_push_r);
    } else {
        compiler->add_op_code(OpCodes::op_push_r);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PredicateArgument
///////////////////////////////////////////////////////////////////////////////
PredicateArgument::PredicateArgument(
    NomadIndex line,
    NomadIndex column,
    NomadId predicate_script_id
):
    Argument(line, column, nullptr),
    m_predicate_script_id(predicate_script_id)
{
}

void PredicateArgument::on_compile(Compiler* compiler, Script* script) {
    compiler->add_index(m_predicate_script_id);
}
///////////////////////////////////////////////////////////////////////////////
// CallbackArgument
///////////////////////////////////////////////////////////////////////////////
CallbackArgument::CallbackArgument(NomadIndex line, NomadIndex column, const Type* type):
    Argument(line, column, type)
{
}

///////////////////////////////////////////////////////////////////////////////
// ScriptCallbackArgument
///////////////////////////////////////////////////////////////////////////////
ScriptCallbackArgument::ScriptCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadString name):
    CallbackArgument(line, column, type),
    m_script_name(std::move(name))
{
}

const NomadString& ScriptCallbackArgument::get_script_name() const {
    return m_script_name;
}

void ScriptCallbackArgument::on_compile(Compiler* compiler, Script* script) {
    auto script_id = compiler->get_runtime()->get_script_id(m_script_name);

    if (script_id == NOMAD_INVALID_ID) {
        compiler->report_error("Unknown callback script name: " + m_script_name);
        return;
    }

    compiler->add_op_code(OpCodes::op_id_push);
    compiler->add_index(script_id);
}

///////////////////////////////////////////////////////////////////////////////
// FunCallbackArgument
///////////////////////////////////////////////////////////////////////////////
FunCallbackArgument::FunCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadId script_id):
    CallbackArgument(line, column, type),
    m_script_id(script_id)
{

}

void FunCallbackArgument::on_compile(Compiler* compiler, Script* script) {
    compiler->add_op_code(OpCodes::op_id_push);
    compiler->add_index(m_script_id);
}

} // namespace nomad