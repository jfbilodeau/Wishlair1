//
// Created by jfbil on 2024-12-07.
//

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Parser.hpp"
#include "nomad/compiler/SyntaxTree.hpp"

#include "nomad/compiler/statements/ReturnStatement.hpp"

#include <memory>

namespace nomad {

class SelectStatementNode : public StatementNode {
public:
    SelectStatementNode(NomadIndex line, NomadIndex column, std::unique_ptr<Expression> expression);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;

};

SelectStatementNode::SelectStatementNode(NomadIndex line, NomadIndex column, std::unique_ptr<Expression> expression):
    StatementNode(line, column),
    m_expression(std::move(expression))
{
}

void SelectStatementNode::onParse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);
}

void SelectStatementNode::onCompile(Compiler* compiler, Script* script) {
    auto expression_type = m_expression->getType();

    if (expression_type != compiler->getRuntime()->getBooleanType()) {
        compiler->reportError("`select` expression must be of type boolean");
    }
}

std::unique_ptr<StatementNode> parseSelectStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto script_name = compiler->generateScriptName("select", script, tokens->getLineIndex());

    auto script_id = compiler->registerScriptSource(
        script_name,
        script->getPath(),
        tokens->getLine()
    );

    auto select_script = compiler->getRuntime()->getScript(script_id);

    auto predicate = parser::parseExpression(compiler, script, tokens);

    // Wrap predicate in return statement.
    auto return_statement = std::make_unique<ReturnStatementNode>(tokens->getLineIndex(), tokens->getColumnIndex(), std::move(predicate));

    // Wrap statement in statement list.
    auto script_node = std::make_unique<ScriptNode>(tokens->getLineIndex(), tokens->getColumnIndex());
    script_node->addStatement(std::move(return_statement));

    // Assign statement list to script
    compiler->setScriptNode(script_id, std::move(script_node));

    auto select_statement = std::make_unique<SelectStatementNode>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        std::move(predicate)
    );

    return select_statement;
}

} // namespace nomad