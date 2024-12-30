//
// Created by jfbil on 2024-10-11.
//

#ifndef NOMAD_ARGUMENT_HPP
#define NOMAD_ARGUMENT_HPP

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

    [[nodiscard]] const Type* get_type() const;

    void generate_code(Compiler* compiler, Script* script);

protected:
    virtual void on_compile(Compiler* compiler, Script* script) = 0;

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
    void on_compile(Compiler* compiler, Script* script) override;

private:
    std::unique_ptr<Expression> m_expression;
};

///////////////////////////////////////////////////////////////////////////////
// PredicateArgument
///////////////////////////////////////////////////////////////////////////////
class PredicateArgument : public Argument {
public:
    PredicateArgument(NomadIndex line, NomadIndex column, NomadId predicate_script_id);

protected:
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadId m_predicate_script_id;
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

    [[nodiscard]] const NomadString& get_script_name() const;

protected:
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadString m_script_name;
};

///////////////////////////////////////////////////////////////////////////////
// FunCallbackArgument
///////////////////////////////////////////////////////////////////////////////
class FunCallbackArgument : public CallbackArgument {
public:
    FunCallbackArgument(NomadIndex line, NomadIndex column, const Type* type, NomadId script_id);

protected:
    void on_compile(Compiler* compiler, Script* script) override;

private:
    NomadId m_script_id;
};


} // namespace nomad

#endif //NOMAD_ARGUMENT_HPP
