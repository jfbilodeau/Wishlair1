//
// Created by jfbil on 2023-06-05.
//

#include "nomad/compiler/StatementCompilers.hpp"

#include "nomad/Nomad.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Tokenizer.hpp"
#include "nomad/script/Runtime.hpp"
#include "nomad/compiler/ScriptBuilder.hpp"
#include "nomad/String.hpp"

namespace nomad {

void compile_fun_body(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    size_t parameter_count = 0;

    while (!parser->end_of_tokens()) {
        auto& token = parser->next_token();

        if (token.type != TokenType::Identifier) {
            builder->report_error("Identifier expected", parser);
        }

        auto& parameter_name = token.text;

        builder->register_script_variable(parameter_name);

        parameter_count++;
    }

    builder->set_parameter_count(parameter_count);

    // Parameters are compiled. Move to first line of script
    parser->next_line();

    compiler->compile_block(parser, builder, {"end"});

    parser->next_line(); // Skip 'end'

    builder->end_script();
}

void compile_fun_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    auto& token = parser->next_token();

    if (token.type != TokenType::Identifier) {
        builder->report_error("Identifier expected", parser);
    }

    auto& fun_name = token.text;

    auto identifier_type = builder->get_identifier_type(fun_name);

    if (identifier_type != IdentifierType::Unknown) {
        builder->report_error("Function name '" + fun_name + "' is already used", parser);
    }

    const String path =
        builder->get_script_name() + "." + fun_name + "[" + std::to_string(parser->get_line_index() + 1) + "]";
    auto fun_id = compiler->register_script_file(fun_name, path, parser->get_source());
    auto function = compiler->get_runtime()->get_script(fun_id);

    ScriptBuilder fun_builder(function, compiler);

    compile_fun_body(compiler, parser, &fun_builder);
}

void compile_return_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    builder->add_instruction("i_return");

    compiler->compile_expression(parser, builder);
}

void compile_assert_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    String line = trim_copy(parser->get_line());
    String assertion_message =
        "Assertion failed: " + builder->get_script_name() + "[" + std::to_string(parser->get_line_index() + 1) + "]: " +
        line;

    NomadId assertion_message_id = compiler->get_runtime()->get_string_id(assertion_message);

    builder->add_instruction("i_assert");
    builder->add_id(assertion_message_id);

    compiler->compile_expression(parser, builder);
}

void compile_string_assert_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    String line = trim_copy(parser->get_line());
    String assertion_message =
        "String assertion failed: " + builder->get_script_name() + "[" + std::to_string(parser->get_line_index() + 1) +
        "]: " + line;

    NomadId assertion_message_id = compiler->get_runtime()->get_string_id(assertion_message);

    builder->add_instruction("i_string_assert");
    builder->add_id(assertion_message_id);

    compiler->compile_string_expression(parser, builder);
    compiler->compile_string_expression(parser, builder);
}

void compile_const_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    auto& constant_token = parser->next_token();
    auto& constant_name = constant_token.text;

    if (constant_token.type != TokenType::Identifier) {
        builder->report_error("Identifier expected. Got '" + constant_name + "' instead", parser);
    }

    parser->expect(TokenType::Operator);

    if (parser->next_token().text != "=") {
        builder->report_error("Expected '=' after Constant name.", parser);
    }

    if (compiler->get_runtime()->get_constant_id(constant_name) != NOMAD_INVALID_ID) {
        builder->report_error("Constant '" + constant_name + "' already exists.", parser);
    }

    auto constant_id = compiler->get_runtime()->register_constant(constant_name, 0);

    builder->add_instruction("i_set_constant");
    builder->add_id(constant_id);
    compiler->compile_expression(parser, builder);
}

void compile_if_statement(Compiler* compiler, Tokenizer* parser, ScriptBuilder* builder) {
    std::vector<size_t> jump_end_indices;

    builder->add_instruction("i_if");
    auto if_jump_instruction_index = builder->add_short(-1); // Placeholder for jump address.

    compiler->compile_expression(parser, builder);

    parser->next_line();

    compiler->compile_block(parser, builder, {"elseIf", "else", "endIf"});

    // Jump to end...
    builder->add_instruction("i_jump");
    auto jump_index = builder->add_short(-1); // Placeholder for jump

    jump_end_indices.push_back(jump_index);

    builder->set_short(if_jump_instruction_index, Short(builder->get_op_code_size()));

    while (parser->token_is("elseIf")) {
        // Consume 'elseIf'
        parser->next_token();

        builder->add_instruction("i_if");
        auto else_if_jump_instruction_index = builder->add_short(-1); // Placeholder for jump address.

        compiler->compile_expression(parser, builder);

        // Skip over 'elseIf' line
        parser->next_line();

        compiler->compile_block(parser, builder, {"elseIf", "else", "endIf"});

        // Jump for previous block to skip over 'elseIf'
        builder->add_instruction("i_jump");
        auto else_if_jump_index = builder->add_short(-1); // Placeholder for jump

        jump_end_indices.push_back(else_if_jump_index);

        // Adjust elseIf statement to jump to next block
        builder->set_short(else_if_jump_instruction_index, Short(builder->get_op_code_size()));
    }

    if (trim_copy(parser->get_line()) == "else") {
        // Skip over 'else'
        parser->next_line();

        compiler->compile_block(parser, builder, {"endIf"});
    }

    if (trim_copy(parser->get_line()) != "endIf") {
        builder->report_error("Expected 'endIf' to close 'if' statement.", parser);
    }

    // Resolve jump addresses to end of if statement
    for (auto index: jump_end_indices) {
        builder->set_short(index, Short(builder->get_op_code_size()));
    }
}

} // nomad