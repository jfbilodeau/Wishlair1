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

const Type* Argument::getType() const {
    return m_type;
}

void Argument::generateCode(Compiler* compiler, Script* script) {
    onCompile(compiler, script);
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

void ExpressionArgument::onCompile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    auto argumentType = getType();
    auto expressionType = m_expression->getType();

    if ( argumentType == nullptr || expressionType == nullptr) {
        compiler->reportError("Cannot determine type of argument");
        return;
    }

    if (!argumentType->same_type(expressionType)) {
        compiler->reportError("Argument type mismatch. Expected " + argumentType->getName() + " but got " + expressionType->getName());
        return;
    }

    if (expressionType == compiler->getRuntime()->getStringType()) {
        compiler->addOpCode(OpCodes::op_string_push_r);
    } else {
        compiler->addOpCode(OpCodes::op_push_r);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PredicateArgument
///////////////////////////////////////////////////////////////////////////////
PredicateArgument::PredicateArgument(
    NomadIndex line,
    NomadIndex column,
    NomadId predicateScriptId
):
    Argument(line, column, nullptr),
    m_predicateScriptId(predicateScriptId)
{
}

void PredicateArgument::onCompile(Compiler* compiler, Script* script) {
    compiler->addIndex(m_predicateScriptId);
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
    m_scriptName(std::move(name))
{
}

const NomadString& ScriptCallbackArgument::getScriptName() const {
    return m_scriptName;
}

void ScriptCallbackArgument::onCompile(Compiler* compiler, Script* script) {
    auto scriptId = compiler->getRuntime()->getScriptId(m_scriptName);

    if (scriptId == NOMAD_INVALID_ID) {
        compiler->reportError("Unknown callback script name: " + m_scriptName);
        return;
    }

    compiler->addOpCode(OpCodes::op_id_push);
    compiler->addIndex(scriptId);
}

///////////////////////////////////////////////////////////////////////////////
// FunCallbackArgument
///////////////////////////////////////////////////////////////////////////////
FunCallbackArgument::FunCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadId scriptId):
    CallbackArgument(line, column, type),
    m_scriptId(scriptId)
{

}

void FunCallbackArgument::onCompile(Compiler* compiler, Script* script) {
    compiler->addOpCode(OpCodes::op_id_push);
    compiler->addIndex(m_scriptId);
}

} // namespace nomad