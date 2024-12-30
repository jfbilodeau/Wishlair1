//
// Created by jfbil on 2023-06-05.
//

#ifndef NOMAD_STATEMENT_COMPILERS_HPP
#define NOMAD_STATEMENT_COMPILERS_HPP

namespace nomad {

// Forward declarations
class Compiler;
class Tokenizer;
class ScriptBuilder;

// Not a command. Just a helper to compile the body of a function
// Used by ::compile_fun_statement and Compiler::compile_script_parameter
void compile_fun_body(Compiler *compiler, Tokenizer *parser, ScriptBuilder *builder);

void compile_fun_statement(Compiler *compiler, Tokenizer *parser, ScriptBuilder *builder);
void compile_return_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder);
void compile_assert_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder);
void compile_string_assert_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder);
void compile_const_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder);
void compile_if_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder);

} // nomad

#endif //NOMAD_STATEMENT_COMPILERS_HPP
