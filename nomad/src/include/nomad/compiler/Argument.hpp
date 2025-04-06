//
// Created by jfbil on 2024-10-11.
//

#pragma once

#include "nomad/compiler/StatementParsers.hpp"
#include "nomad/compiler/SyntaxTree.hpp"

namespace nomad {

class FunStatementNode;

///////////////////////////////////////////////////////////////////////////////
// Argument
///////////////////////////////////////////////////////////////////////////////
class Argument : public AstNode {
public:
    Argument(NomadIndex line, NomadIndex column, const Type* type);
    ~Argument() override = default;

    [[nodiscard]] const Type* getType() const;

    void generateCode(Compiler* compiler, Script* script);

protected:
    virtual void onCompile(Compiler* compiler, Script* script) = 0;

private:
    const Type* m_type = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
// ExpressionArgument
///////////////////////////////////////////////////////////////////////////////
class ExpressionArgument : public Argument {
public:
    ExpressionArgument(NomadIndex line, NomadIndex column, const Type* type, std::unique_ptr<Expression> expression);

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;
};

///////////////////////////////////////////////////////////////////////////////
// PredicateArgument
///////////////////////////////////////////////////////////////////////////////
class PredicateArgument : public Argument {
public:
    PredicateArgument(NomadIndex line, NomadIndex column, NomadId predicateScriptId);

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadId m_predicateScriptId;
};

///////////////////////////////////////////////////////////////////////////////
// CallbackArgument
///////////////////////////////////////////////////////////////////////////////
class CallbackArgument : public Argument {
public:
    CallbackArgument(NomadIndex line, NomadIndex column, const Type* type);
};

///////////////////////////////////////////////////////////////////////////////
// ScriptCallbackArgument
///////////////////////////////////////////////////////////////////////////////
class ScriptCallbackArgument : public CallbackArgument {
public:
    ScriptCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadString name);

    [[nodiscard]] const NomadString& getScriptName() const;

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadString m_scriptName;
};

///////////////////////////////////////////////////////////////////////////////
// FunCallbackArgument
///////////////////////////////////////////////////////////////////////////////
class FunCallbackArgument : public CallbackArgument {
public:
    FunCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadId scriptId);

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    NomadId m_scriptId;
};


} // namespace nomad

