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

NomadIndex AstException::get_line() const {
    return m_line;
}

NomadIndex AstException::get_column() const {
    return m_column;
}


///////////////////////////////////////////////////////////////////////////////
// AstNode
AstNode::AstNode(NomadIndex line, NomadIndex column):
    m_line(line),
    m_column(column) {
}

void AstNode::raise_exception(const NomadString& message) const {
    throw AstException(message, m_line, m_column);
}

NomadIndex AstNode::get_line() const {
    return m_line;
}

NomadIndex AstNode::get_column() const {
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
        (*argument)->generate_code(compiler, script);
    }
}

///////////////////////////////////////////////////////////////////////////////
// StatementNode
StatementNode::StatementNode(NomadIndex line, NomadIndex column):
    AstNode(line, column) {
}

void StatementNode::parse(Compiler* compiler, Script* script) {
    on_parse(compiler, script);
}

void StatementNode::compile(Compiler* compiler, Script* script) {
    on_compile(compiler, script);
}

void StatementNode::on_parse(Compiler* compiler, Script* script) {
    // Default implementation does nothing
}

void StatementNode::on_compile(Compiler* compiler, Script* script) {
    // Default implementation does nothing
}

///////////////////////////////////////////////////////////////////////////////
// NullStatementNode
NullStatementNode::NullStatementNode(NomadIndex line, NomadIndex column):
    StatementNode(line, column) {

}

void NullStatementNode::on_compile(nomad::Compiler* compiler, Script* script) {
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

void AssignmentStatementNode::on_parse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);

    auto expression_type = m_expression->get_type();

    if (expression_type == nullptr) {
        raise_exception("Cannot determine type of assignment expression");
    }

    IdentifierDefinition identifier;
    compiler->get_identifier_definition(m_identifier, script, identifier);

    if (identifier.identifier_type == IdentifierType::Unknown) {
        // New script variable
        auto variable_id = script->register_variable(
            m_identifier,
            expression_type
        );

        if (variable_id == NOMAD_INVALID_ID) {
            raise_exception("Cannot register variable '" + m_identifier + "'");
        }
    } else if (identifier.identifier_type == IdentifierType::ContextVariable) {
        auto context = compiler->get_runtime()->get_variable_context(identifier.context_id);

        context->register_variable(m_identifier, expression_type);
    }
}

void AssignmentStatementNode::on_compile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    IdentifierDefinition identifier;

    compiler->get_identifier_definition(m_identifier, script, identifier);

    switch (identifier.identifier_type) {
        case IdentifierType::ScriptVariable: {
            auto expression_type = m_expression->get_type();
            auto variable_type = script->get_variable_type(identifier.variable_id);

            if (variable_type == nullptr) {
                script->set_variable_type(identifier.variable_id, expression_type);
            } else if (variable_type != expression_type) {
                raise_exception("Cannot assign value of type '" + expression_type->get_name() + "' to variable '" + m_identifier + "' of type '" + variable_type->get_name() + "'");
            }

            if (expression_type == compiler->get_runtime()->get_string_type()) {
                compiler->add_op_code(OpCodes::op_script_variable_string_set);
            } else {
                compiler->add_op_code(OpCodes::op_script_variable_set);
            }
            compiler->add_id(identifier.variable_id);

            break;
        }
        case IdentifierType::DynamicVariable:
            if (m_expression->get_type() == nullptr) {
                raise_exception("Cannot determine type of assignment expression");
            }

            if (m_expression->get_type() != compiler->get_runtime()->get_dynamic_variable_type(identifier.variable_id)) {
                raise_exception("Cannot assign value of type '" + m_expression->get_type()->get_name() + "' to dynamic variable '" + m_identifier + "' of type '" + compiler->get_runtime()->get_dynamic_variable_type(identifier.variable_id)->get_name() + "'");
            }

            if (m_expression->get_type() == compiler->get_runtime()->get_string_type()) {
                compiler->add_op_code(OpCodes::op_dynamic_variable_string_set);
            } else {
                compiler->add_op_code(OpCodes::op_dynamic_variable_set);
            }
            compiler->add_id(identifier.variable_id);
            break;
        case IdentifierType::ContextVariable: {
            auto context = compiler->get_runtime()->get_variable_context(identifier.context_id);

            auto variable_type = context->get_variable_type(identifier.variable_id);

            if (variable_type == nullptr) {
                context->register_variable(m_identifier, m_expression->get_type());
            } else if (variable_type != m_expression->get_type()) {
                raise_exception("Cannot assign value of type '" + m_expression->get_type()->get_name() + "' to variable '" + m_identifier + "' of type '" + variable_type->get_name() + "'");
            }

            if (m_expression->get_type() == compiler->get_runtime()->get_string_type()) {
                compiler->add_op_code(OpCodes::op_context_variable_string_set);
            } else {
                compiler->add_op_code(OpCodes::op_context_variable_set);
            }
            compiler->add_id(identifier.context_id);
            compiler->add_id(identifier.variable_id);
            break;
        }
        case IdentifierType::Constant:
            raise_exception("Cannot assign value to constant '" + m_identifier + "'");
        case IdentifierType::Keyword:
            raise_exception("Cannot assign value to keyword '" + m_identifier + "'");
        case IdentifierType::Statement:
            raise_exception("Cannot assign to use statement '" + m_identifier + "'e");
        case IdentifierType::Command:
            raise_exception("Cannot assign to command '" + m_identifier + "'");
        case IdentifierType::Script:
            raise_exception("Cannot assign to script '" + m_identifier + "'e");
        default:
            raise_exception("Cannot assign to unknown identifier '" + m_identifier + "'");
    }
}

///////////////////////////////////////////////////////////////////////////////
// CommandStatementNode
CommandStatementNode::CommandStatementNode(NomadIndex line, NomadIndex column, NomadString name):
    StatementNode(line, column),
    m_name(std::move(name)) {
}

void CommandStatementNode::on_compile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler,script);

    auto command_id = compiler->get_runtime()->get_command_id(m_name);

    if (command_id == NOMAD_INVALID_ID) {
        raise_exception("Unknown command '" + m_name + "'");
    }

    compiler->add_command_call(command_id);
}


void CommandStatementNode::add_argument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* CommandStatementNode::get_arguments() {
    return &m_arguments;
}

///////////////////////////////////////////////////////////////////////////////
// ScriptCallStatementNode
ScriptCallStatementNode::ScriptCallStatementNode(NomadIndex line, NomadIndex column, NomadString name):
    StatementNode(line, column),
    m_name(std::move(name)) {
}

void ScriptCallStatementNode::on_compile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler, script);

    auto target_script_id = compiler->get_runtime()->get_script_id(m_name);

    compiler->add_script_call(target_script_id);
}

void ScriptCallStatementNode::add_argument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* ScriptCallStatementNode::get_arguments() {
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

void StatementList::add_statement(std::unique_ptr<StatementNode> statement) {
    if (statement == nullptr) {
        // Skip null statements
        return;
    }

    m_statements.push_back(std::move(statement));
}

NomadIndex StatementList::get_statement_count() const {
    return m_statements.size();
}

NomadIndex StatementList::is_empty() const {
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

void ScriptNode::add_statement(std::unique_ptr<StatementNode> statement) {
    m_statements.add_statement(std::move(statement));
}

StatementList* ScriptNode::get_statements() {
    return &m_statements;
}

} // namespace nomad