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
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;

};

SelectStatementNode::SelectStatementNode(NomadIndex line, NomadIndex column, std::unique_ptr<Expression> expression):
    StatementNode(line, column),
    m_expression(std::move(expression))
{
}

void SelectStatementNode::on_parse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);
}

void SelectStatementNode::on_compile(Compiler* compiler, Script* script) {
    auto expression_type = m_expression->get_type();

    if (expression_type != compiler->get_runtime()->get_boolean_type()) {
        compiler->report_error("`select` expression must be of type boolean");
    }
}

std::unique_ptr<StatementNode> parse_select_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto script_name = compiler->generate_script_name("select", script, tokens->get_line_index());

    auto script_id = compiler->register_script_source(
        script_name,
        script->get_path(),
        tokens->get_line()
    );

    auto select_script = compiler->get_runtime()->get_script(script_id);

    auto predicate = parser::parse_expression(compiler, script, tokens);

    // Wrap predicate in return statement.
    auto return_statement = std::make_unique<ReturnStatementNode>(tokens->get_line_index(), tokens->get_column_index(), std::move(predicate));

    // Wrap statement in statement list.
    auto script_node = std::make_unique<ScriptNode>(tokens->get_line_index(), tokens->get_column_index());
    script_node->add_statement(std::move(return_statement));

    // Assign statement list to script
    compiler->set_script_node(script_id, std::move(script_node));

    auto select_statement = std::make_unique<SelectStatementNode>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        std::move(predicate)
    );

    return select_statement;
}

} // namespace nomad