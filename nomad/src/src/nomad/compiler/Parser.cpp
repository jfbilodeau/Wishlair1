//
// Created by jfbil on 2023-10-08.
//

#include "nomad/system/String.hpp"

#include "nomad/compiler/Parser.hpp"

#include "nomad/compiler/Argument.hpp"
#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Expression.hpp"
#include "nomad/compiler/StatementParsers.hpp"
#include "nomad/compiler/SyntaxTree.hpp"
#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/compiler/statements/ReturnStatement.hpp"

namespace nomad {

ParserException::ParserException(const NomadString& message, NomadIndex row, NomadIndex column):
    NomadException("Parse error[" + toString(row) + ":" + toString(column) + "]: " + message),
    m_row(row),
    m_column(column) {
}

namespace parser {

void throwParserError(const NomadString& message) {
    throw ParserException(message, 0, 0);
}

void throwParserError(const NomadString& message, Tokenizer* tokens) {
    throw ParserException(message, tokens->getLineIndex(), tokens->getColumnIndex());
}

using ParseFunction = std::unique_ptr<Expression> (*)(Compiler*, Script*, Tokenizer*);

std::unique_ptr<Expression> parseBinaryOperatorExpression(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    std::vector<BinaryOperator> operators,
    ParseFunction nextParseFn
) {
    auto expression = nextParseFn(compiler, script, tokens);

    while (true) {
        if (!tokens->tokenIs(TokenType::Operator)) {
            return expression;
        }

        auto& operatorToken = tokens->currentToken();

        BinaryOperator op = getBinaryOperator(operatorToken.textValue);

        auto opIterator = std::find(operators.begin(), operators.end(), op);

        if (opIterator != operators.end()) {
            tokens->nextToken();

            expression = std::make_unique<BinaryExpression>(
                tokens->getLineIndex(),
                tokens->getColumnIndex(),
                op,
                std::move(expression),
                nextParseFn(compiler, script, tokens)
            );
        } else {
            return expression;
        }
    }
}

std::unique_ptr<StatementNode> parseLine(Compiler* compiler, Script* script, Tokenizer* tokenizer) {
    if (tokenizer->endOfLine()) {
        // Empty line
        return nullptr;
    }

    if (tokenizer->getTokenCount() >= 3 && tokenizer->getTokenAt(1) == "=") {
        return parseAssignmentStatement(compiler, script, tokenizer);
    }

    return parseStatement(compiler, script, tokenizer);
}

void parseBlock(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const std::vector<NomadString>& endTokens,
    StatementList* statements
) {
    while (true) {
        if (tokens->isEndOfFile()) {
            throwParserError("Unexpected end of file", tokens);
        }

        auto& token = tokens->currentToken();

        if (std::find(endTokens.begin(), endTokens.end(), token.textValue) != endTokens.end()) {
            // Consumes the end token
            tokens->nextToken();

            return;
        }

        auto statement = parseLine(compiler, script, tokens);

        statements->addStatement(std::move(statement));

        tokens->nextLine();
    }
}

std::unique_ptr<StatementNode> parseStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& statementToken = tokens->nextToken();
    auto& statementName = statementToken.textValue;

    ParseStatementFn statementFn;

    if (compiler->getParseStatementFn(statementName, statementFn)) {
        if (statementFn == nullptr) {
            return nullptr;
        }

        auto currentLine = tokens->getLineIndex();

        auto statement = statementFn(compiler, script, tokens);

        if (tokens->getLineIndex() == currentLine) {
            expectEndOfLine(compiler, script, tokens);
        }

        return statement;
    }

    CommandDefinition command;
    if (compiler->getRuntime()->getCommandDefinition(statementName, command)) {
        auto statement = parseCommandStatement(compiler, script, tokens, command);

        expectEndOfLine(compiler, script, tokens);

        return statement;
    }

    auto scriptId = compiler->getRuntime()->getScriptId(statementName);

    if (scriptId != NOMAD_INVALID_ID) {
        auto statement = parseScriptCall(compiler, script, tokens, statementName);

        expectEndOfLine(compiler, script, tokens);

        return statement;
    }

    throwParserError("Unknown statement, command or script name: '" + statementName + "'", tokens);
}

std::unique_ptr<StatementNode> parseCommandStatement(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command) {
    auto line = tokens->getLineIndex();
    auto column = tokens->getColumnIndex();

    auto commandStatement = std::make_unique<CommandStatementNode>(
        line,
        column,
        command.name
    );

    parseCommandArguments(compiler, script, tokens, command, commandStatement->getArguments());

    return commandStatement;
}

std::unique_ptr<StatementNode> parseScriptCall(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const NomadString& scriptName
) {
    auto scriptCallExpression = std::make_unique<ScriptCallStatementNode>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        scriptName
    );

    auto scriptId = compiler->getRuntime()->getScriptId(scriptName);

    if (scriptId == NOMAD_INVALID_ID) {
        throwParserError("Unknown script name: '" + scriptName + "'", tokens);
    }

    auto targetScript = compiler->getRuntime()->getScript(scriptId);

    parseScriptArguments(compiler, script, tokens, targetScript, scriptCallExpression->getArguments());

    return scriptCallExpression;
}

std::unique_ptr<StatementNode> parseReturnStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto expression = parseExpression(compiler, script, tokens);

    return std::make_unique<ReturnStatementNode>(tokens->getLineIndex(), tokens->getColumnIndex(), std::move(expression));
}

std::unique_ptr<Expression> parseExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseLogicalAndExpression(compiler, script, tokens);
}

std::unique_ptr<Expression> parseLogicalAndExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens,
        {BinaryOperator::AndAnd},
        &parseLogicalOrExpression
    );
}


std::unique_ptr<Expression> parseLogicalOrExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens,
        {BinaryOperator::PipePipe},
        &parseRelationalExpression
    );
}

std::unique_ptr<Expression> parseRelationalExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::EqualEqual,
            BinaryOperator::BangEqual,
            BinaryOperator::LessThan,
            BinaryOperator::LessThanEqual,
            BinaryOperator::GreaterThan,
            BinaryOperator::GreaterThanEqual,
        },
        &parseBitwiseAndExpression
    );
}

std::unique_ptr<Expression> parseBitwiseAndExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::And,
        },
        &parseBitwiseXorExpression
    );
}

std::unique_ptr<Expression> parseBitwiseXorExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Caret,
        },
        &parseBitwiseOrExpression
    );
}

std::unique_ptr<Expression> parseBitwiseOrExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Pipe,
        },
        &parseTermExpression
    );
}

std::unique_ptr<Expression> parseTermExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Plus,
            BinaryOperator::Minus,
        },
        &parseProductExpression
    );
}

std::unique_ptr<Expression> parseProductExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parseBinaryOperatorExpression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Star,
            BinaryOperator::Slash,
            BinaryOperator::Percent,
        },
        &parseParenthesesExpression
    );
}

std::unique_ptr<Expression> parseParenthesesExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (tokens->tokenIs("(")) {
        tokens->nextToken();

        auto expression = parseExpression(compiler, script, tokens);

        if (!tokens->tokenIs(")")) {
            throwParserError("Expected ')'", tokens);
        }

        tokens->nextToken();

        return expression;
    }

    return parseUnaryOperatorExpression(compiler, script, tokens);
}

std::unique_ptr<Expression> parseUnaryOperatorExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto token = tokens->currentToken();

    auto op = getUnaryOperator(token.textValue);

    if (op == UnaryOperator::Unknown) {
        return parsePrimaryExpression(compiler, script, tokens);
    }

    tokens->nextToken();

    if (tokens->currentToken().textValue == "(") {
        tokens->nextToken();

        auto expression = parseExpression(compiler, script, tokens);

        if (!tokens->tokenIs(")")) {
            throwParserError("Expected ')'", tokens);
        }

        tokens->nextToken();

        return std::make_unique<UnaryExpression>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            op,
            std::move(expression)
        );
    } else {
        auto expression = parsePrimaryExpression(compiler, script, tokens);

        return std::make_unique<UnaryExpression>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            op,
            std::move(expression)
        );
    }
}

void expectEndOfLine(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (!tokens->endOfLine()) {
        throwParserError("Expected end of line", tokens);
    }
}

IdentifierType getIdentifierType(Compiler* compiler, const NomadString& name, Script* script) {
    if (compiler->getRuntime()->getKeywordId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Keyword;
    }

    if (compiler->isStatement(name)) {
        return IdentifierType::Statement;
    }

    CommandDefinition commandDefinition;

    if (compiler->getRuntime()->getCommandDefinition(name, commandDefinition)) {
        return IdentifierType::Command;
    }

    if (compiler->getRuntime()->getConstantId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Constant;
    }

    if (compiler->getRuntime()->getScriptId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Script;
    }

    if (compiler->getRuntime()->getDynamicVariableId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::DynamicVariable;
    }

    if (compiler->getRuntime()->getVariableContextIdByPrefix(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ContextVariable;
    }

    if (script && script->getVariableId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ScriptVariable;
    }

    return IdentifierType::Unknown;
}

std::unique_ptr<StatementNode> parseAssignmentStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& variableName = tokens->currentToken().textValue;

    IdentifierDefinition identifierDefinition;
    compiler->getIdentifierDefinition(variableName, script, identifierDefinition);

    if (identifierDefinition.identifierType == IdentifierType::Unknown) {
        script->registerVariable(variableName, nullptr);
    }

    tokens->nextToken(); // Consume variable name
    tokens->expect("=");  // Consume '='

    auto expression = parseExpression(compiler, script, tokens);

    expectEndOfLine(compiler, script, tokens);

    return std::make_unique<AssignmentStatementNode>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        variableName,
        std::move(expression)
    );
}

std::unique_ptr<Expression> parsePrimaryExpression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (tokens->endOfLine()) {
        throwParserError("Unexpected end of line", tokens);
    }

    auto& token = tokens->nextToken();

    if (token.type == TokenType::FormatString) {
        return std::make_unique<FormatStringLiteral>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            token.textValue
        );
    }

    if (token.type == TokenType::String) {
        return std::make_unique<StringLiteral>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            token.textValue
        );
    }

    if (token.type == TokenType::Integer) {
        return std::make_unique<IntegerLiteral>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            token.integerValue
        );
    }

    if (token.type == TokenType::Float) {
        return std::make_unique<FloatLiteral>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            token.floatValue
        );
    }

    auto& identifier = token.textValue;

    if (token.type == TokenType::Identifier) {
        // Is it a command?
        CommandDefinition command;
        if (compiler->getRuntime()->getCommandDefinition(token.textValue, command)) {
            if (command.returnType->isVoid()) {
                throwParserError(
                    "Cannot use command '" + token.textValue + "' in an expression because it does not return a value",
                    tokens
                );
            }

            return parseCommandCallExpression(compiler, script, tokens, command);
        }

        // Is it a script?
        auto scriptId = compiler->getRuntime()->getScriptId(token.textValue);

        if (scriptId != NOMAD_INVALID_ID) {
            return parseScriptCallExpression(compiler, script, tokens, token.textValue);
        }

        // Constant?
        auto constantId = compiler->getRuntime()->getConstantId(token.textValue);

        if (constantId != NOMAD_INVALID_ID) {
            return std::make_unique<ConstantValueExpression>(
                tokens->getLineIndex(),
                tokens->getColumnIndex(),
                token.textValue
            );
        }

        // Identifier
        return std::make_unique<IdentifierExpression>(
            tokens->getLineIndex(),
            tokens->getColumnIndex(),
            token.textValue
        );
    }

    throwParserError("Unexpected token '" + token.textValue + "'", tokens);
}

std::unique_ptr<Expression> parseScriptCallExpression(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const NomadString& scriptName
) {
    auto scriptCallExpression = std::make_unique<ScriptCallExpression>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        scriptName
    );

    auto scriptId = compiler->getRuntime()->getScriptId(scriptName);
    auto targetScript = compiler->getRuntime()->getScript(scriptId);
    auto parameterCount = targetScript->getParameterCount();

    for (auto i = 0; i < parameterCount; ++i) {
        if (tokens->endOfLine()) {
            NomadString message =
                "Expected arguments " +
                toString(parameterCount) +
                " for call to script '" +
                scriptName +
                "' but only got "
                + toString(scriptCallExpression->getArgumentCount()) +
                " arguments";

            throwParserError(message, tokens);
        }

        auto expression = parseExpression(compiler, script, tokens);

        scriptCallExpression->addArgument(std::move(expression));
    }

    return scriptCallExpression;
}

void parseScriptArguments(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    Script* targetScript,
    ArgumentList* arguments
) {
    auto parameterCount = targetScript->getParameterCount();

    for (auto i = 0; i < parameterCount; ++i) {
        auto& parameterName = targetScript->getParameterName(i);
        auto parameterType = targetScript->getParameterType(i);

        if (tokens->endOfLine()) {
            throwParserError("Expected arguments '" + parameterName + "'", tokens);
        }

        auto argumentType = parameterType;

        auto callbackType = argumentType->asCallback();

        if (callbackType) {
            // TODO: add support for script callback
            tokens->expect("fun");

            auto scriptName = generateFunScriptName(compiler, script, tokens->getLineIndex());
            auto& scriptPath = script->getPath();
            auto& scriptSource = script->getSource();

            auto scriptId = compiler->registerScriptSource(scriptName, scriptPath, scriptSource);

            auto funScript = compiler->getRuntime()->getScript(scriptId);

            parseCallbackFunParameters(compiler, funScript, tokens, callbackType);

            tokens->expectEndOfLine();

            auto funBody = parseFunBody(compiler, funScript, tokens);

            compiler->setScriptNode(scriptId, std::move(funBody));

            auto funArgument = std::make_unique<FunCallbackArgument>(
                tokens->getLineIndex(),
                tokens->getColumnIndex(),
                argumentType,
                scriptId
            );

            arguments->add(std::move(funArgument));
        } else {
            auto argument = parseExpressionArgument(compiler, argumentType, script, tokens);

            arguments->add(std::move(argument));
        }
    }
}

std::unique_ptr<Expression> parseCommandCallExpression(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command) {
    auto commandExpression = std::make_unique<CallCommandExpression>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        command.name
    );

    parseCommandArguments(compiler, script, tokens, command, commandExpression->getArguments());

    return commandExpression;
}

void parseCommandArguments(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const CommandDefinition& command,
    ArgumentList* arguments
) {
    for (auto& parameter: command.parameters) {
        if (tokens->endOfLine()) {
            throwParserError("Expected arguments '" + parameter.name + "'", tokens);
        }

        auto argumentType = parameter.type;

        auto callbackType = argumentType->asCallback();

        if (callbackType) {
            // TODO: add support for named script callback
            tokens->expect("fun");

            auto scriptName = generateFunScriptName(compiler, script, tokens->getLineIndex());
            auto& scriptPath = script->getPath();
            auto& scriptSource = script->getSource();

            auto scriptId = compiler->registerScriptSource(scriptName, scriptPath, scriptSource);

            auto funScript = compiler->getRuntime()->getScript(scriptId);

            parseCallbackFunParameters(compiler, funScript, tokens, callbackType);

            tokens->expectEndOfLine();

            auto funBody = parseFunBody(compiler, funScript, tokens);

            compiler->setScriptNode(scriptId, std::move(funBody));

            auto funArgument = std::make_unique<FunCallbackArgument>(
                tokens->getLineIndex(),
                tokens->getColumnIndex(),
                argumentType,
                scriptId
            );

            arguments->add(std::move(funArgument));

            continue;
        } else if (argumentType == compiler->getRuntime()->getPredicateType()) {
            auto argument = parsePredicateArgument(compiler, script, tokens);

            arguments->add(std::move(argument));
        } else {
            auto argument = parseExpressionArgument(compiler, argumentType, script, tokens);

            arguments->add(std::move(argument));
        }
    }
}

std::unique_ptr<ExpressionArgument> parseExpressionArgument(
    Compiler* compiler,
    const Type* argumentType,
    Script* script,
    Tokenizer* tokens
) {
    auto expression = parseExpression(compiler, script, tokens);

    return std::make_unique<ExpressionArgument>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        argumentType,
        std::move(expression)
    );
}

std::unique_ptr<PredicateArgument> parsePredicateArgument(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens
) {
    auto predicateScriptName = compiler->generateScriptName("predicate", script, tokens->getLineIndex());

    auto predicateScriptId = compiler->registerScriptSource(
        predicateScriptName,
        script->getPath(),
        tokens->getLine()
    );

    auto predicateScript = compiler->getRuntime()->getScript(predicateScriptId);

    auto predicateExpression = parser::parseExpression(compiler, script, tokens);

    // Wrap predicate in return statement.
    auto returnStatement = std::make_unique<ReturnStatementNode>(tokens->getLineIndex(), tokens->getColumnIndex(), std::move(predicateExpression));

    // Wrap statement in statement list.
    auto scriptNode = std::make_unique<ScriptNode>(tokens->getLineIndex(), tokens->getColumnIndex());
    scriptNode->addStatement(std::move(returnStatement));

    // Assign statement list to script
    compiler->setScriptNode(predicateScriptId, std::move(scriptNode));

    auto predicateArgument = std::make_unique<PredicateArgument>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
        predicateScriptId
    );

    return predicateArgument;
}

} // namespace parser

} // namespace nomad