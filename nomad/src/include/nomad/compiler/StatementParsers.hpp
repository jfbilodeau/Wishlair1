//
// Created by jfbil on 2023-06-05.
//

#ifndef NOMAD_STATEMENT_COMPILERS_HPP
#define NOMAD_STATEMENT_COMPILERS_HPP

#include "nomad/nomad.hpp"

#include "nomad/compiler/SyntaxTree.hpp"

#include <memory>
#include <vector>

namespace nomad {

// Forward declarations
class Compiler;
class FunStatementNode;
class Tokenizer;
class ScriptBuilder;
class StatementNode;
class StatementList;

NomadString generate_fun_script_name(Compiler* compiler, Script* script, NomadIndex line);

class FunStatementNode : public StatementNode {
public:
    FunStatementNode(
        NomadIndex col, NomadIndex row, Script* script, std::unique_ptr<Expression> ptr
    );

    void add_parameter(const NomadString& parameter_name);
    [[nodiscard]] NomadIndex get_parameter_count() const;

    [[nodiscard]] Script* get_script() const;
    [[nodiscard]] const NomadString& get_script_name() const;
    StatementList& get_body();

protected:
    void on_compile(Compiler* compiler, Script* script) override;

private:
    Script* m_script;
    std::vector<NomadString> m_parameters;
    StatementList m_body;
};

// Compile the parameter list of a `fun`
void parse_fun_parameters(Script* script, Compiler* compiler, Tokenizer* tokens);

// Compile the parameter list of a callback `fun`
void parse_callback_fun_parameters(Compiler* compiler, Script* script, Tokenizer* tokens, const ScriptType* callback_type);

// Not a command. Just a helper to compile the body of a function
// Used by ::compile_fun_statement and Compiler::compile_script_parameter
std::unique_ptr<ScriptNode> parse_fun_body(Compiler* compiler, Script* script, Tokenizer* tokens);

// Statement parsers
void pre_parse_fun_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parse_fun_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
//std::unique_ptr<StatementNode> parse_return_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parse_assert_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
void pre_parse_const_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parse_if_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
void pre_parse_params_statement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // nomad

#endif //NOMAD_STATEMENT_COMPILERS_HPP
