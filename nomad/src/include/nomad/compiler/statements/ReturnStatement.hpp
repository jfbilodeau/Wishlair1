//
// Created by jfbil on 2024-12-11.
//

#pragma once

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
    void onCompile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;
};

std::unique_ptr<StatementNode> parseReturnStatement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // namespace nomad

