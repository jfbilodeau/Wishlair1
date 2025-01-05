//
// Created by jfbil on 2024-12-11.
//

#ifndef NOMAD_RETURNSTATEMENT_HPP
#define NOMAD_RETURNSTATEMENT_HPP

#include "nomad/nomad.hpp"

#include "nomad/compiler/SyntaxTree.hpp"

#include <memory>

namespace nomad {

// Forward declaration
class Compiler;
class Expression;
class Script;
class StatementNode;
class SyntaxTree;
class Tokenizer;

class ReturnStatementNode : public StatementNode {
public:
    explicit ReturnStatementNode(NomadIndex line, NomadIndex column, std::unique_ptr<Expression> expression);

protected:
    void on_compile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;
};

std::unique_ptr<StatementNode> parse_return_statement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // namespace nomad

#endif //NOMAD_RETURNSTATEMENT_HPP
