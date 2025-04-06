//
// Created by jfbil on 2024-10-07.
//

#pragma once

#include "nomad/compiler/SyntaxTree.hpp"

#include "nomad/nomad.hpp"

namespace nomad {
///////////////////////////////////////////////////////////////////////////////
// Unary expressions
///////////////////////////////////////////////////////////////////////////////
class UnaryExpression : public Expression {
public:
    explicit UnaryExpression(NomadIndex line, NomadIndex column, UnaryOperator unaryOperator, std::unique_ptr<Expression> expression);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

    UnaryOperator m_unaryOperator;
    std::unique_ptr<Expression> m_expression;
};

///////////////////////////////////////////////////////////////////////////////
// Binary expressions
///////////////////////////////////////////////////////////////////////////////
class BinaryExpression : public Expression {
public:
    BinaryExpression(NomadIndex line, NomadIndex column, BinaryOperator binaryOperator, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

    BinaryOperator m_binaryOperator;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
};

///////////////////////////////////////////////////////////////////////////////
// Call expressions
///////////////////////////////////////////////////////////////////////////////
class CallCommandExpression : public Expression {
public:
    explicit CallCommandExpression(NomadIndex line, NomadIndex column, NomadString name);

    void addArgument(std::unique_ptr<Argument> argument);

    [[nodiscard]] ArgumentList* getArguments();

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_name;
    ArgumentList m_arguments;
};

class ScriptCallExpression : public Expression {
public:
    explicit ScriptCallExpression(NomadIndex line, NomadIndex column, NomadString name);

    void addArgument(std::unique_ptr<Expression> argument);

    [[nodiscard]] NomadIndex getArgumentCount() const;

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_name;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

///////////////////////////////////////////////////////////////////////////////
// Primary expressions
///////////////////////////////////////////////////////////////////////////////
class PrimaryExpression : public Expression {
public:
    PrimaryExpression(NomadIndex line, NomadIndex column);
    ~PrimaryExpression() override = default;
};

class BooleanLiteral : public PrimaryExpression {
public:
    BooleanLiteral(NomadIndex line, NomadIndex column, bool m_value);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadBoolean m_value;
};

class IntegerLiteral : public PrimaryExpression {
public:
    IntegerLiteral(NomadIndex line, NomadIndex column, NomadInteger value);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadInteger m_value;
};

class FloatLiteral : public PrimaryExpression {
public:
    FloatLiteral(NomadIndex line, NomadIndex column, NomadFloat value);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadFloat m_value;
};

class StringLiteral : public PrimaryExpression {
public:
    StringLiteral(NomadIndex line, NomadIndex column, NomadString value);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_value;
};

class FormatStringLiteral : public Expression {
public:
    FormatStringLiteral(NomadIndex line, NomadIndex column, const NomadString& formatString);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_formatString;
};

class ConstantValueExpression : public PrimaryExpression {
public:
    explicit ConstantValueExpression(NomadIndex line, NomadIndex column, const NomadString& identifier);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_constantName;
    NomadId m_constantId = NOMAD_INVALID_ID;
};

///////////////////////////////////////////////////////////////////////////////
// Identifier expressions
///////////////////////////////////////////////////////////////////////////////
class IdentifierExpression : public PrimaryExpression {
public:
    explicit IdentifierExpression(NomadIndex line, NomadIndex column, const NomadString& identifier);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_identifier;
    IdentifierDefinition m_identifierDefinition = {};
};

} // namespace nomad