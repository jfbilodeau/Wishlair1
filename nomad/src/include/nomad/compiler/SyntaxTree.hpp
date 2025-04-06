//
// Created by jfbil on 2023-09-30.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/compiler/Operators.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/FormatString.hpp"

#include <memory>
#include <vector>

namespace nomad {

// Forward declarations
class Argument;
class AstNode;
class Compiler;
class FunStatementNode;  // Defined in StatementParsers.hpp
class Runtime;
class ScriptBuilder;

// Forward declarations in file
class CallCommandExpression;
class Expression;
class StatementNode;
class StatementList;

///////////////////////////////////////////////////////////////////////////////
// AST exception
///////////////////////////////////////////////////////////////////////////////
class AstException : public NomadException {
public:
    explicit AstException(const NomadString& message, NomadIndex line, NomadIndex column);

    [[nodiscard]] NomadIndex getLine() const;
    [[nodiscard]] NomadIndex getColumn() const;

private:
    NomadIndex m_line;
    NomadIndex m_column;
};

///////////////////////////////////////////////////////////////////////////////
// Abstract syntax tree
///////////////////////////////////////////////////////////////////////////////
class AstNode {
public:
    AstNode(NomadIndex line, NomadIndex column);
    AstNode(const AstNode&) = delete;
    virtual ~AstNode() = default;

    [[nodiscard]] NomadIndex getLine() const;
    [[nodiscard]] NomadIndex getColumn() const;

protected:
    [[noreturn]] void raiseException(const NomadString& message) const;

private:
    NomadIndex m_line;
    NomadIndex m_column;
};

///////////////////////////////////////////////////////////////////////////////
// Expressions
///////////////////////////////////////////////////////////////////////////////
class Expression : public AstNode {
public:
    Expression(NomadIndex line, NomadIndex column, const Type* type = nullptr);
    ~Expression() override;
    void parse(Compiler* compiler, Script* script);
    void compile(Compiler* compiler, Script* script);

    [[nodiscard]] const Type* getType() const;
    [[nodiscard]] const ScriptValue& getValue() const;
    [[nodiscard]] bool hasType() const;
    [[nodiscard]] bool hasValue() const;
    [[nodiscard]] bool isParsed() const;

protected:
    void setResolved(const ScriptValue& value, const Type* type);
    void setType(const Type* type);
    void setValue(const ScriptValue& value);

    virtual void onParse(Compiler* compiler, Script* script) = 0;
    virtual void onCompile(Compiler* compiler, Script* script) = 0;

private:
    ScriptValue m_value;
    const Type* m_type = nullptr;
    bool m_hasValue = false;
};

class ArgumentList {
public:
    void add(std::unique_ptr<Argument> argument);

    void compile(Compiler* compiler, Script* script);

protected:

private:
    std::vector<std::unique_ptr<Argument>> m_arguments;
};

///////////////////////////////////////////////////////////////////////////////
// Statements
///////////////////////////////////////////////////////////////////////////////

class StatementNode : public AstNode {
public:
    StatementNode(NomadIndex line, NomadIndex column);
    ~StatementNode() override = default;

    void parse(Compiler* compiler, Script* script);
    void compile(Compiler* compiler, Script* script);

protected:
    virtual void onParse(Compiler* compiler, Script* script);
    virtual void onCompile(Compiler* compiler, Script* script);
};

class NullStatementNode : public StatementNode {
public:
    NullStatementNode(NomadIndex line, NomadIndex column);

protected:
    void onCompile(Compiler* compiler, Script* script) override;
};

class AssignmentStatementNode : public StatementNode {
public:
    AssignmentStatementNode(NomadIndex line, NomadIndex column, NomadString identifier, std::unique_ptr<Expression> expression);

protected:
    void onParse(Compiler* compiler, Script* script) override;
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_identifier;
    std::unique_ptr<Expression> m_expression;
};

class CommandStatementNode : public StatementNode {
public:
    CommandStatementNode(NomadIndex line, NomadIndex column, NomadString name);

    void addArgument(std::unique_ptr<Argument> argument);

    [[nodiscard]] ArgumentList* getArguments();

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_name;
    ArgumentList m_arguments;
};

class ScriptCallStatementNode : public StatementNode {
public:
    ScriptCallStatementNode(NomadIndex line, NomadIndex column, NomadString name);

    void addArgument(std::unique_ptr<Argument> argument);

    [[nodiscard]] ArgumentList* getArguments();

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_name;
    ArgumentList m_arguments;
};

///////////////////////////////////////////////////////////////////////////////
// Statement list (Not an AST node)
///////////////////////////////////////////////////////////////////////////////
class StatementList {
public:
    StatementList() = default;
    StatementList(StatementList&&) = default;

    StatementList& operator=(StatementList&&) = default;

    void parse(Compiler* compiler, Script* script);
    void compile(Compiler* compiler, Script* script);

    void addStatement(std::unique_ptr<StatementNode> statement);

    [[nodiscard]] NomadIndex getStatementCount() const;
    [[nodiscard]] NomadIndex isEmpty() const;

private:
    std::vector<std::unique_ptr<StatementNode>> m_statements;
};

///////////////////////////////////////////////////////////////////////////////
// Script and program
///////////////////////////////////////////////////////////////////////////////

class ScriptNode : public AstNode {
public:
    ScriptNode(NomadIndex line, NomadIndex row);

    void parse(Compiler* compiler, Script* script);
    void compile(Compiler* compiler, Script* script);

    void addStatement(std::unique_ptr<StatementNode> statement);
    [[nodiscard]] StatementList* getStatements();

private:
    StatementList m_statements;
};

} // nomad

