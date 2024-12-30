//
// Created by jfbil on 2024-12-11.
//

#include "nomad/compiler/statements/ReturnStatement.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/SyntaxTree.hpp"
#include "nomad/compiler/Tokenizer.hpp"

namespace nomad {

ReturnStatementNode::ReturnStatementNode(
    NomadIndex line,
    NomadIndex column,
    std::unique_ptr<Expression> expression
):
    StatementNode(line, column),
    m_expression(std::move(expression))
{
}

void ReturnStatementNode::on_compile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    compiler->add_op_code(OpCodes::op_return);

    if (script->get_return_type() == nullptr) {
        script->set_return_type(m_expression->get_type());
    } else if (script->get_return_type() != m_expression->get_type()) {
        compiler->report_error("Return type mismatch in script '" + script->get_name() + "'. Script return type is '" + script->get_return_type()->get_name() + "' but expression type is '" + m_expression->get_type()->get_name() + "'");
    }
}

std::unique_ptr<StatementNode> parse_return_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto expression = parser::parse_expression(compiler, script, tokens);
    
    return std::make_unique<ReturnStatementNode>(tokens->get_line_index(), tokens->get_column_index(), std::move(expression));
}

} // namespace nomad