//
// Created by jfbil on 2023-09-30.
//

#include "nomad/compiler/SyntaxTree.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/FormatStringCompiler.hpp"

#include <cmath>

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// AstException
AstException::AstException(const NomadString& message, NomadIndex line, NomadIndex column):
    NomadException(message),
    m_line(line),
    m_column(column) {
}

NomadIndex AstException::getLine() const {
    return m_line;
}

NomadIndex AstException::getColumn() const {
    return m_column;
}


///////////////////////////////////////////////////////////////////////////////
// AstNode
AstNode::AstNode(NomadIndex line, NomadIndex column):
    m_line(line),
    m_column(column) {
}

void AstNode::raiseException(const NomadString& message) const {
    throw AstException(message, m_line, m_column);
}

NomadIndex AstNode::getLine() const {
    return m_line;
}

NomadIndex AstNode::getColumn() const {
    return m_column;
}

///////////////////////////////////////////////////////////////////////////////
// ArgumentList
void ArgumentList::add(std::unique_ptr<Argument> argument) {
    m_arguments.push_back(std::move(argument));
}

void ArgumentList::compile(Compiler* compiler, Script* script) {
    // Push arguments in reverse order
    for (auto argument = m_arguments.rbegin(); argument != m_arguments.rend(); ++argument) {
        (*argument)->generateCode(compiler, script);
    }
}

///////////////////////////////////////////////////////////////////////////////
// StatementNode
StatementNode::StatementNode(NomadIndex line, NomadIndex column):
    AstNode(line, column) {
}

void StatementNode::parse(Compiler* compiler, Script* script) {
    onParse(compiler, script);
}

void StatementNode::compile(Compiler* compiler, Script* script) {
    onCompile(compiler, script);
}

void StatementNode::onParse(Compiler* compiler, Script* script) {
    // Default implementation does nothing
}

void StatementNode::onCompile(Compiler* compiler, Script* script) {
    // Default implementation does nothing
}

///////////////////////////////////////////////////////////////////////////////
// NullStatementNode
NullStatementNode::NullStatementNode(NomadIndex line, NomadIndex column):
    StatementNode(line, column) {

}

void NullStatementNode::onCompile(nomad::Compiler* compiler, Script* script) {
    // Nothing to do...
}

///////////////////////////////////////////////////////////////////////////////
// AssignmentStatementNode
AssignmentStatementNode::AssignmentStatementNode(
    NomadIndex line,
    NomadIndex column,
    NomadString identifier,
    std::unique_ptr<Expression> expression
):
    StatementNode(line, column),
    m_identifier(std::move(identifier)),
    m_expression(std::move(expression)) {

}

void AssignmentStatementNode::onParse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);

    auto expression_type = m_expression->getType();

    // if (expression_type == nullptr) {
    //     raise_exception("Cannot determine type of assignment expression");
    // }

    IdentifierDefinition identifier;
    compiler->getIdentifierDefinition(m_identifier, script, identifier);

    if (identifier.identifierType == IdentifierType::Unknown) {
        // New script variable
        auto variable_id = script->registerVariable(
            m_identifier,
            expression_type
        );

        if (variable_id == NOMAD_INVALID_ID) {
            raiseException("Cannot register variable '" + m_identifier + "'");
        }
    } else if (identifier.identifierType == IdentifierType::ContextVariable) {
        auto context = compiler->getRuntime()->getVariableContext(identifier.contextId);

        context->registerVariable(m_identifier, expression_type);
    }
}

void AssignmentStatementNode::onCompile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    IdentifierDefinition identifier;

    compiler->getIdentifierDefinition(m_identifier, script, identifier);

    switch (identifier.identifierType) {
        case IdentifierType::ScriptVariable: {
            auto expression_type = m_expression->getType();
            auto variable_type = script->getVariableType(identifier.variableId);

            if (variable_type == nullptr) {
                script->set_variable_type(identifier.variableId, expression_type);
            } else if (variable_type != expression_type) {
                raiseException("Cannot assign value of type '" + expression_type->getName() + "' to variable '" + m_identifier + "' of type '" + variable_type->getName() + "'");
            }

            if (expression_type == compiler->getRuntime()->getStringType()) {
                compiler->addOpCode(OpCodes::op_script_variable_string_set);
            } else {
                compiler->addOpCode(OpCodes::op_script_variable_set);
            }
            compiler->addId(identifier.variableId);

            break;
        }
        case IdentifierType::DynamicVariable:
            if (m_expression->getType() == nullptr) {
                raiseException("Cannot determine type of assignment expression");
            }

            if (m_expression->getType() != compiler->getRuntime()->getDynamicVariableType(identifier.variableId)) {
                raiseException("Cannot assign value of type '" + m_expression->getType()->getName() + "' to dynamic variable '" + m_identifier + "' of type '" + compiler->getRuntime()->getDynamicVariableType(identifier.variableId)->getName() + "'");
            }

            if (m_expression->getType() == compiler->getRuntime()->getStringType()) {
                compiler->addOpCode(OpCodes::op_dynamic_variable_string_set);
            } else {
                compiler->addOpCode(OpCodes::op_dynamic_variable_set);
            }
            compiler->addId(identifier.variableId);
            break;
        case IdentifierType::ContextVariable: {
            auto context = compiler->getRuntime()->getVariableContext(identifier.contextId);

            auto variable_type = context->getVariableType(identifier.variableId);

            if (variable_type == nullptr) {
                context->registerVariable(m_identifier, m_expression->getType());
            } else if (variable_type != m_expression->getType()) {
                raiseException("Cannot assign value of type '" + m_expression->getType()->getName() + "' to variable '" + m_identifier + "' of type '" + variable_type->getName() + "'");
            }

            if (m_expression->getType() == compiler->getRuntime()->getStringType()) {
                compiler->addOpCode(OpCodes::op_context_variable_string_set);
            } else {
                compiler->addOpCode(OpCodes::op_context_variable_set);
            }
            compiler->addId(identifier.contextId);
            compiler->addId(identifier.variableId);
            break;
        }
        case IdentifierType::Constant:
            raiseException("Cannot assign value to constant '" + m_identifier + "'");
        case IdentifierType::Keyword:
            raiseException("Cannot assign value to keyword '" + m_identifier + "'");
        case IdentifierType::Statement:
            raiseException("Cannot assign to use statement '" + m_identifier + "'e");
        case IdentifierType::Command:
            raiseException("Cannot assign to command '" + m_identifier + "'");
        case IdentifierType::Script:
            raiseException("Cannot assign to script '" + m_identifier + "'e");
        default:
            raiseException("Cannot assign to unknown identifier '" + m_identifier + "'");
    }
}

///////////////////////////////////////////////////////////////////////////////
// CommandStatementNode
CommandStatementNode::CommandStatementNode(NomadIndex line, NomadIndex column, NomadString name):
    StatementNode(line, column),
    m_name(std::move(name)) {
}

void CommandStatementNode::onCompile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler,script);

    auto command_id = compiler->getRuntime()->getCommandId(m_name);

    if (command_id == NOMAD_INVALID_ID) {
        raiseException("Unknown command '" + m_name + "'");
    }

    compiler->addCommandCall(command_id);
}


void CommandStatementNode::addArgument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* CommandStatementNode::getArguments() {
    return &m_arguments;
}

///////////////////////////////////////////////////////////////////////////////
// ScriptCallStatementNode
ScriptCallStatementNode::ScriptCallStatementNode(NomadIndex line, NomadIndex column, NomadString name):
    StatementNode(line, column),
    m_name(std::move(name)) {
}

void ScriptCallStatementNode::onCompile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler, script);

    auto target_script_id = compiler->getRuntime()->getScriptId(m_name);

    compiler->addScriptCall(target_script_id);
}

void ScriptCallStatementNode::addArgument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* ScriptCallStatementNode::getArguments() {
    return &m_arguments;
}

///////////////////////////////////////////////////////////////////////////////
// StatementList
void StatementList::parse(Compiler* compiler, Script* script) {
    for (auto& statement: m_statements) {
        statement->parse(compiler, script);
    }
}

void StatementList::compile(Compiler* compiler, Script* script) {
    for (auto& statement: m_statements) {
        statement->compile(compiler, script);
    }
}

void StatementList::addStatement(std::unique_ptr<StatementNode> statement) {
    if (statement == nullptr) {
        // Skip null statements
        return;
    }

    m_statements.push_back(std::move(statement));
}

NomadIndex StatementList::getStatementCount() const {
    return m_statements.size();
}

NomadIndex StatementList::isEmpty() const {
    return m_statements.empty();
}

///////////////////////////////////////////////////////////////////////////////
// ScriptNode
ScriptNode::ScriptNode(NomadIndex line, NomadIndex row):
    AstNode(line, row)
{}

void ScriptNode::parse(Compiler* compiler, Script* script) {
    m_statements.parse(compiler, script);
}

void ScriptNode::compile(Compiler* compiler, Script* script) {
    m_statements.compile(compiler, script);
}

void ScriptNode::addStatement(std::unique_ptr<StatementNode> statement) {
    m_statements.addStatement(std::move(statement));
}

StatementList* ScriptNode::getStatements() {
    return &m_statements;
}

} // namespace nomad