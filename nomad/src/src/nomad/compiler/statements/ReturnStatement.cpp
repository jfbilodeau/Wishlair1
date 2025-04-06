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

void ReturnStatementNode::onCompile(Compiler* compiler, Script* script) {
    m_expression->compile(compiler, script);

    compiler->addOpCode(OpCodes::op_return);

    if (script->getReturnType() == nullptr) {
        script->setReturnType(m_expression->getType());
    } else if (script->getReturnType() != m_expression->getType()) {
        compiler->reportError("Return type mismatch in script '" + script->getName() + "'. Script return type is '" + script->getReturnType()->getName() + "' but expression type is '" + m_expression->getType()->getName() + "'");
    }
}

std::unique_ptr<StatementNode> parseReturnStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto expression = parser::parseExpression(compiler, script, tokens);
    
    return std::make_unique<ReturnStatementNode>(tokens->getLineIndex(), tokens->getColumnIndex(), std::move(expression));
}

} // namespace nomad