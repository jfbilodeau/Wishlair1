//
// Created by jfbil on 2023-10-08.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/compiler/Argument.hpp"
#include "nomad/compiler/Expression.hpp"

#include "nomad/compiler/SyntaxTree.hpp"

namespace nomad {

// Forward declaration
class Compiler;
class Tokenizer;

using PreParseStatementFn = void (*)(Compiler*, Script*, Tokenizer*);
using ParseStatementFn = std::unique_ptr<StatementNode> (*)(Compiler*, Script*, Tokenizer*);

class ParserException: public NomadException {
public:
    explicit ParserException(const NomadString& message, NomadIndex row = 0, NomadIndex column = 0);

    [[nodiscard]] NomadIndex getRow() const { return m_row; }
    [[nodiscard]] NomadIndex getColumn() const { return m_column; }

private:
    NomadIndex m_row;
    NomadIndex m_column;
};

namespace parser {

[[noreturn]] void throwParserError(const NomadString& message);
[[noreturn]] void throwParserError(const NomadString& message, Tokenizer* tokens);

void expectEndOfLine(Compiler* compiler, Script* script, Tokenizer* tokens);

IdentifierType getIdentifierType(Compiler* compiler, const NomadString& name, Script* script = nullptr);

std::unique_ptr<StatementNode> parseLine(Compiler* compiler, Script* script, Tokenizer* tokenizer);
void parseBlock(Compiler* compiler, Script* script, Tokenizer* tokens, const std::vector<NomadString>& endTokens, StatementList* statements);
std::unique_ptr<StringLiteral> parseStringExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parseAssignmentStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parseStatement(Compiler* compiler, Script* script, Tokenizer* tokenizer);
std::unique_ptr<StatementNode> parseCommandStatement(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command);
void parseScriptParameter(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandParameterDefinition& parameter);
std::unique_ptr<StatementNode> parseScriptCall(Compiler* compiler, Script* script, Tokenizer* tokens, const NomadString& scriptName);
std::unique_ptr<Expression> parseLogicalAndExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseLogicalOrExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseRelationalExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseBitwiseAndExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseBitwiseXorExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseBitwiseOrExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseTermExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseProductExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseParenthesesExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseUnaryOperatorExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parsePrimaryExpression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parseScriptCallExpression(Compiler* compiler, Script* script, Tokenizer* tokens, const NomadString& scriptName);
void parseScriptArguments(Compiler* compiler, Script* script, Tokenizer* tokens, Script* targetScript, ArgumentList* arguments);
std::unique_ptr<Expression> parseCommandCallExpression(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command);
void parseCommandArguments(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command, ArgumentList* arguments);
std::unique_ptr<ExpressionArgument> parseExpressionArgument(Compiler* compiler, const Type* argumentType, Script* script, Tokenizer* tokens);
std::unique_ptr<PredicateArgument> parsePredicateArgument(Compiler* compiler, Script* script, Tokenizer* tokens);

} // namespace parser

} // namespace nomad
