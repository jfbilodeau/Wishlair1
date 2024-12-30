//
// Created by jfbil on 2024-10-07.
//

#ifndef NOMAD_EXPRESSION_HPP
#define NOMAD_EXPRESSION_HPP

#include "nomad/compiler/SyntaxTree.hpp"

#include "nomad/Nomad.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Unary expressions
///////////////////////////////////////////////////////////////////////////////
class UnaryExpression : public Expression {
public:
    explicit UnaryExpression(NomadIndex line, NomadIndex column, UnaryOperator unary_operator, std::unique_ptr<Expression> expression);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

    UnaryOperator m_unary_operator;
    std::unique_ptr<Expression> m_expression;
};

///////////////////////////////////////////////////////////////////////////////
// Binary expressions
///////////////////////////////////////////////////////////////////////////////
class BinaryExpression : public Expression {
public:
    BinaryExpression(NomadIndex line, NomadIndex column, BinaryOperator binary_operator, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

    BinaryOperator m_binary_operator;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
};

//class StringLiteralExpression : public Expression {
//public:
//    explicit StringLiteralExpression(NomadIndex line, NomadIndex column, NomadString value);
//    ~StringLiteralExpression() override;
//
//protected:
//    void on_parse(Compiler* compiler) override;
//    void on_compile(Compiler* compiler) override;
//
//private:
//    NomadString m_value;
//    NomadId m_string_id;
//};

///////////////////////////////////////////////////////////////////////////////
// Call expressions
///////////////////////////////////////////////////////////////////////////////
class CallCommandExpression : public Expression {
public:
    explicit CallCommandExpression(NomadIndex line, NomadIndex column, NomadString name);

    void add_argument(std::unique_ptr<Argument> argument);

    [[nodiscard]] ArgumentList* get_arguments();

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_name;
    ArgumentList m_arguments;
};

class ScriptCallExpression : public Expression {
public:
    explicit ScriptCallExpression(NomadIndex line, NomadIndex column, NomadString name);

    void add_argument(std::unique_ptr<Expression> argument);

    [[nodiscard]] NomadIndex get_argument_count() const;

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

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
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadBoolean m_value;
};

class IntegerLiteral : public PrimaryExpression {
public:
    IntegerLiteral(NomadIndex line, NomadIndex column, NomadInteger value);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadInteger m_value;
};

class FloatLiteral : public PrimaryExpression {
public:
    FloatLiteral(NomadIndex line, NomadIndex column, NomadFloat value);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadFloat m_value;
};

class StringLiteral : public PrimaryExpression {
public:
    StringLiteral(NomadIndex line, NomadIndex column, NomadString value);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_value;
};

class FormatStringLiteral : public Expression {
public:
    FormatStringLiteral(NomadIndex line, NomadIndex column, const NomadString& format_string);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_format_string;
};

class ConstantValueExpression : public PrimaryExpression {
public:
    explicit ConstantValueExpression(NomadIndex line, NomadIndex column, const NomadString& identifier);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_constant_name;
    NomadId m_constant_id = NOMAD_INVALID_ID;
};

///////////////////////////////////////////////////////////////////////////////
// Identifier expressions
///////////////////////////////////////////////////////////////////////////////
class IdentifierExpression : public PrimaryExpression {
public:
    explicit IdentifierExpression(NomadIndex line, NomadIndex column, const NomadString& identifier);

protected:
    void on_parse(Compiler* compiler, Script* script) override;
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_identifier;
    IdentifierDefinition m_identifier_definition = {};
};

///////////////////////////////////////////////////////////////////////////////
// CallbackExpression
///////////////////////////////////////////////////////////////////////////////
//class CallbackExpression : public Expression {
//public:
//    explicit CallbackExpression(NomadIndex line, NomadIndex column);
//};

///////////////////////////////////////////////////////////////////////////////
// ScriptCallbackExpression
///////////////////////////////////////////////////////////////////////////////
//class ScriptCallbackExpression : public CallbackExpression {
//    ScriptCallbackExpression(NomadIndex line, NomadIndex column, NomadString script_name);
//
//protected:
//    void on_parse(Compiler* compiler) override;
//    void on_compile(Compiler* compiler) override;
//
//private:
//    NomadString m_script_name;
//};

/////////////////////////////////////////////////////////////////////////////////
//// FunCallbackExpression
/////////////////////////////////////////////////////////////////////////////////
//class FunCallbackExpression : public CallbackExpression {
//    FunCallbackExpression(NomadIndex line, NomadIndex column);
//
//protected:
//    bool on_resolve(Compiler* compiler) override;
//    void on_compile(Compiler* compiler) override;
//
//private:
//    NomadId m_script_id;
//    Script* m_script;
//};

} // namespace nomad

#endif //NOMAD_EXPRESSION_HPP
