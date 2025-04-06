//
// Created by jfbil on 2023-06-05.
//

#pragma once

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

NomadString generateFunScriptName(Compiler* compiler, Script* script, NomadIndex line);

class FunStatementNode : public StatementNode {
public:
    FunStatementNode(
        NomadIndex col, NomadIndex row, Script* script, std::unique_ptr<Expression> ptr
    );

    void addParameter(const NomadString& parameter_name);
    [[nodiscard]] NomadIndex getParameterCount() const;

    [[nodiscard]] Script* getScript() const;
    [[nodiscard]] const NomadString& getScriptName() const;
    StatementList& getBody();

protected:
    void onCompile(Compiler* compiler, Script* script) override;

private:
    Script* m_script;
    std::vector<NomadString> m_parameters;
    StatementList m_body;
};

// Compile the parameter list of a `fun`
void parseFunParameters(Script* script, Compiler* compiler, Tokenizer* tokens);

// Compile the parameter list of a callback `fun`
void parseCallbackFunParameters(Compiler* compiler, Script* script, Tokenizer* tokens, const ScriptType* callbackType);

// Not a command. Just a helper to compile the body of a function
std::unique_ptr<ScriptNode> parseFunBody(Compiler* compiler, Script* script, Tokenizer* tokens);

// Statement parsers
void preParseFunStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parseFunStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parseAssertStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
void preParseConstStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parseIfStatement(Compiler* compiler, Script* script, Tokenizer* tokens);
void preParseParamsStatement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // nomad

