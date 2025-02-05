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
    NomadException("Parse error[" + to_string(row) + ":" + to_string(column) + "]: " + message),
    m_row(row),
    m_column(column) {
}

namespace parser {

void throw_parser_error(const NomadString& message) {
    throw ParserException(message, 0, 0);
}

void throw_parser_error(const NomadString& message, Tokenizer* tokens) {
    throw ParserException(message, tokens->get_line_index(), tokens->get_column_index());
}

using ParseFunction = std::unique_ptr<Expression> (*)(Compiler*, Script*, Tokenizer*);

std::unique_ptr<Expression> parse_binary_operator_expression(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    std::vector<BinaryOperator> operators,
    ParseFunction next_parse_fn
) {
    auto expression = next_parse_fn(compiler, script, tokens);

    while (true) {
        if (!tokens->token_is(TokenType::Operator)) {
            return expression;
        }

        auto& operator_token = tokens->current_token();

        BinaryOperator op = get_binary_operator(operator_token.text_value);

        auto op_iterator = std::find(operators.begin(), operators.end(), op);

        if (op_iterator != operators.end()) {
            tokens->next_token();

            expression = std::make_unique<BinaryExpression>(
                tokens->get_line_index(),
                tokens->get_column_index(),
                op,
                std::move(expression),
                next_parse_fn(compiler, script, tokens)
            );
        } else {
            return expression;
        }
    }
}

std::unique_ptr<StatementNode> parse_line(Compiler* compiler, Script* script, Tokenizer* tokenizer) {
    if (tokenizer->end_of_line()) {
        // Empty line
        return nullptr;
    }

    if (tokenizer->get_token_count() >= 3 && tokenizer->get_token_at(1) == "=") {
        return parse_assignment_statement(compiler, script, tokenizer);
    }

    return parse_statement(compiler, script, tokenizer);
}

void parse_block(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const std::vector<NomadString>& end_tokens,
    StatementList* statements
) {
    while (true) {
        if (tokens->end_of_file()) {
            throw_parser_error("Unexpected end of file", tokens);
        }

        auto& token = tokens->current_token();

        if (std::find(end_tokens.begin(), end_tokens.end(), token.text_value) != end_tokens.end()) {
            // Consumes the end token
            tokens->next_token();

            return;
        }

        auto statement = parse_line(compiler, script, tokens);

        statements->add_statement(std::move(statement));

        tokens->next_line();
    }
}

std::unique_ptr<StatementNode> parse_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& statement_token = tokens->next_token();
    auto& statement_name = statement_token.text_value;

    ParseStatementFn statement_fn;

    if (compiler->get_parse_statement_fn(statement_name, statement_fn)) {
        if (statement_fn == nullptr) {
            return nullptr;
        }

        auto current_line = tokens->get_line_index();

        auto statement = statement_fn(compiler, script, tokens);

        if (tokens->get_line_index() == current_line) {
            expect_end_of_line(compiler, script, tokens);
        }

        return statement;
    }

    CommandDefinition command;
    if (compiler->get_runtime()->get_command_definition(statement_name, command)) {
        auto statement = parse_command_statement(compiler, script, tokens, command);

        expect_end_of_line(compiler, script, tokens);

        return statement;
    }

    auto script_id = compiler->get_runtime()->get_script_id(statement_name);

    if (script_id != NOMAD_INVALID_ID) {
        auto statement = parse_script_call(compiler, script, tokens, statement_name);

        expect_end_of_line(compiler, script, tokens);

        return statement;
    }

    throw_parser_error("Unknown statement, command or script name: '" + statement_name + "'", tokens);
}

std::unique_ptr<StatementNode> parse_command_statement(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command) {
    auto line = tokens->get_line_index();
    auto column = tokens->get_column_index();

    auto command_statement = std::make_unique<CommandStatementNode>(
        line,
        column,
        command.name
    );

    parse_command_arguments(compiler, script, tokens, command, command_statement->get_arguments());

    return command_statement;
}

std::unique_ptr<StatementNode> parse_script_call(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const NomadString& script_name
) {
    auto script_call_expression = std::make_unique<ScriptCallStatementNode>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        script_name
    );

    auto script_id = compiler->get_runtime()->get_script_id(script_name);

    if (script_id == NOMAD_INVALID_ID) {
        throw_parser_error("Unknown script name: '" + script_name + "'", tokens);
    }

    auto target_script = compiler->get_runtime()->get_script(script_id);

    parse_script_arguments(compiler, script, tokens, target_script, script_call_expression->get_arguments());

    return script_call_expression;
}

std::unique_ptr<StatementNode> parse_return_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto expression = parse_expression(compiler, script, tokens);

    return std::make_unique<ReturnStatementNode>(tokens->get_line_index(), tokens->get_column_index(), std::move(expression));
}

std::unique_ptr<Expression> parse_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_logical_and_expression(compiler, script, tokens);
}

std::unique_ptr<Expression> parse_logical_and_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens,
        {BinaryOperator::AndAnd},
        &parse_logical_or_expression
    );
}


std::unique_ptr<Expression> parse_logical_or_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens,
        {BinaryOperator::PipePipe},
        &parse_relational_expression
    );
}

std::unique_ptr<Expression> parse_relational_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
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
        &parse_bitwise_and_expression
    );
}

std::unique_ptr<Expression> parse_bitwise_and_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens, {
            BinaryOperator::And,
        },
        &parse_bitwise_xor_expression
    );
}

std::unique_ptr<Expression> parse_bitwise_xor_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Caret,
        },
        &parse_bitwise_or_expression
    );
}

std::unique_ptr<Expression> parse_bitwise_or_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Pipe,
        },
        &parse_term_expression
    );
}

std::unique_ptr<Expression> parse_term_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Plus,
            BinaryOperator::Minus,
        },
        &parse_product_expression
    );
}

std::unique_ptr<Expression> parse_product_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    return parse_binary_operator_expression(
        compiler,
        script,
        tokens, {
            BinaryOperator::Star,
            BinaryOperator::Slash,
            BinaryOperator::Percent,
        },
        &parse_parentheses_expression
    );
}

std::unique_ptr<Expression> parse_parentheses_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (tokens->token_is("(")) {
        tokens->next_token();

        auto expression = parse_expression(compiler, script, tokens);

        if (!tokens->token_is(")")) {
            throw_parser_error("Expected ')'", tokens);
        }

        tokens->next_token();

        return expression;
    }

    return parse_unary_operator_expression(compiler, script, tokens);
}

std::unique_ptr<Expression> parse_unary_operator_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto token = tokens->current_token();

    auto op = get_unary_operator(token.text_value);

    if (op == UnaryOperator::Unknown) {
        return parse_primary_expression(compiler, script, tokens);
    }

    tokens->next_token();

    auto expression = parse_expression(compiler, script, tokens);

    return std::make_unique<UnaryExpression>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        op,
        std::move(expression)
    );
}

void expect_end_of_line(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (!tokens->end_of_line()) {
        throw_parser_error("Expected end of line", tokens);
    }

//    tokens->next_line();
}

IdentifierType get_identifier_type(Compiler* compiler, const NomadString& name, Script* script) {
    if (compiler->get_runtime()->get_keyword_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Keyword;
    }

    if (compiler->is_statement(name)) {
        return IdentifierType::Statement;
    }

    CommandDefinition commandDefinition;

    if (compiler->get_runtime()->get_command_definition(name, commandDefinition)) {
        return IdentifierType::Command;
    }

    if (compiler->get_runtime()->get_constant_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Constant;
    }

    if (compiler->get_runtime()->get_script_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Script;
    }

    if (compiler->get_runtime()->get_dynamic_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::DynamicVariable;
    }

    if (compiler->get_runtime()->get_variable_context_id_by_prefix(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ContextVariable;
    }

    if (script && script->get_variable_id(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ScriptVariable;
    }

    return IdentifierType::Unknown;
}

std::unique_ptr<StatementNode> parse_assignment_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& variable_name = tokens->current_token().text_value;

    IdentifierDefinition identifier_definition;
    compiler->get_identifier_definition(variable_name, script, identifier_definition);

    if (identifier_definition.identifier_type == IdentifierType::Unknown) {
        script->register_variable(variable_name, nullptr);
    }

    tokens->next_token(); // Consume variable name
    tokens->expect("=");  // Consume '='

    auto expression = parse_expression(compiler, script, tokens);

    expect_end_of_line(compiler, script, tokens);

    return std::make_unique<AssignmentStatementNode>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        variable_name,
        std::move(expression)
    );
}

std::unique_ptr<Expression> parse_primary_expression(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (tokens->end_of_line()) {
        throw_parser_error("Unexpected end of line", tokens);
    }

    auto& token = tokens->next_token();

    if (token.type == TokenType::FormatString) {
        return std::make_unique<FormatStringLiteral>(
            tokens->get_line_index(),
            tokens->get_column_index(),
            token.text_value
        );
    }

    if (token.type == TokenType::String) {
        return std::make_unique<StringLiteral>(
            tokens->get_line_index(),
            tokens->get_column_index(),
            token.text_value
        );
    }

    if (token.type == TokenType::Integer) {
        return std::make_unique<IntegerLiteral>(
            tokens->get_line_index(),
            tokens->get_column_index(),
            token.integer_value
        );
    }

    if (token.type == TokenType::Float) {
        return std::make_unique<FloatLiteral>(
            tokens->get_line_index(),
            tokens->get_column_index(),
            token.float_value
        );
    }

    auto& identifier = token.text_value;

//    auto identifier_type = get_identifier_type(compiler, identifier);

    if (token.type == TokenType::Identifier) {
        // Is it a command?
        CommandDefinition command;
        if (compiler->get_runtime()->get_command_definition(token.text_value, command)) {
            if (command.return_type->is_void()) {
                throw_parser_error(
                    "Cannot use command '" + token.text_value + "' in an expression because it does not return a value",
                    tokens
                );
            }

            return parse_command_call_expression(compiler, script, tokens, command);
        }

        // Is it a script?
        auto script_id = compiler->get_runtime()->get_script_id(token.text_value);

        if (script_id != NOMAD_INVALID_ID) {
            return parse_script_call_expression(compiler, script, tokens, token.text_value);
        }

        // Constant?
        auto constant_id = compiler->get_runtime()->get_constant_id(token.text_value);

        if (constant_id != NOMAD_INVALID_ID) {
            return std::make_unique<ConstantValueExpression>(
                tokens->get_line_index(),
                tokens->get_column_index(),
                token.text_value
            );
        }

        // Identifier
        return std::make_unique<IdentifierExpression>(
            tokens->get_line_index(),
            tokens->get_column_index(),
            token.text_value
        );
    }

    throw_parser_error("Unexpected token '" + token.text_value + "'", tokens);
}

std::unique_ptr<Expression> parse_script_call_expression(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const NomadString& script_name
) {
    auto script_call_expression = std::make_unique<ScriptCallExpression>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        script_name
    );

    auto script_id = compiler->get_runtime()->get_script_id(script_name);
    auto target_script = compiler->get_runtime()->get_script(script_id);
    auto parameter_count = target_script->get_parameter_count();

    for (auto i = 0; i < parameter_count; ++i) {
        if (tokens->end_of_line()) {
            NomadString message =
                "Expected arguments " +
                to_string(parameter_count) +
                " for call to script '" +
                script_name +
                "' but only got "
                + to_string(script_call_expression->get_argument_count()) +
                " arguments";

            throw_parser_error(message, tokens);
        }

        auto expression = parse_expression(compiler, script, tokens);

        script_call_expression->add_argument(std::move(expression));
    }

    return script_call_expression;
}

void parse_script_arguments(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    Script* target_script,
    ArgumentList* arguments
) {
    auto parameter_count = target_script->get_parameter_count();

    for (auto i = 0; i < parameter_count; ++i) {
        auto& parameter_name = target_script->get_parameter_name(i);
        auto parameter_type = target_script->get_parameter_type(i);

        if (tokens->end_of_line()) {
            throw_parser_error("Expected arguments '" + parameter_name + "'", tokens);
        }

        auto argument_type = parameter_type;

        auto callback_type = argument_type->as_callback();

        if (callback_type) {
            // TODO: add support for script callback
            tokens->expect("fun");

            auto script_name = generate_fun_script_name(compiler, script, tokens->get_line_index());
            auto& script_path = script->get_path();
            auto& script_source = script->get_source();

            auto script_id = compiler->register_script_source(script_name, script_path, script_source);

            auto fun_script = compiler->get_runtime()->get_script(script_id);

            parse_callback_fun_parameters(compiler, fun_script, tokens, callback_type);

            tokens->expect_end_of_line();

            auto fun_body = parse_fun_body(compiler, fun_script, tokens);

            compiler->set_script_node(script_id, std::move(fun_body));

            auto fun_argument = std::make_unique<FunCallbackArgument>(
                tokens->get_line_index(),
                tokens->get_column_index(),
                argument_type,
                script_id
            );

            arguments->add(std::move(fun_argument));
        } else {
            auto argument = parse_expression_argument(compiler, argument_type, script, tokens);

            arguments->add(std::move(argument));
        }
    }
}

std::unique_ptr<Expression>
parse_command_call_expression(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command) {
    auto command_expression = std::make_unique<CallCommandExpression>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        command.name
    );

    parse_command_arguments(compiler, script, tokens, command, command_expression->get_arguments());

    return command_expression;
}

void parse_command_arguments(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens,
    const CommandDefinition& command,
    ArgumentList* arguments
) {
    for (auto& parameter: command.parameters) {
        if (tokens->end_of_line()) {
            throw_parser_error("Expected arguments '" + parameter.name + "'", tokens);
        }

        auto argument_type = parameter.type;

        auto callback_type = argument_type->as_callback();

        if (callback_type) {
            // TODO: add support for script callback
            tokens->expect("fun");

            auto script_name = generate_fun_script_name(compiler, script, tokens->get_line_index());
            auto& script_path = script->get_path();
            auto& script_source = script->get_source();

            auto script_id = compiler->register_script_source(script_name, script_path, script_source);

            auto fun_script = compiler->get_runtime()->get_script(script_id);

            parse_callback_fun_parameters(compiler, fun_script, tokens, callback_type);

            tokens->expect_end_of_line();

            auto fun_body = parse_fun_body(compiler, fun_script, tokens);

            compiler->set_script_node(script_id, std::move(fun_body));

            auto fun_argument = std::make_unique<FunCallbackArgument>(
                tokens->get_line_index(),
                tokens->get_column_index(),
                argument_type,
                script_id
            );

            arguments->add(std::move(fun_argument));

            continue;
        } else if (argument_type == compiler->get_runtime()->get_predicate_type()) {
            auto argument = parse_predicate_argument(compiler, script, tokens);

            arguments->add(std::move(argument));
        } else {
            auto argument = parse_expression_argument(compiler, argument_type, script, tokens);

            arguments->add(std::move(argument));
        }
    }
}

std::unique_ptr<ExpressionArgument> parse_expression_argument(
    Compiler* compiler,
    const Type* argument_type,
    Script* script,
    Tokenizer* tokens
) {
    auto expression = parse_expression(compiler, script, tokens);

    return std::make_unique<ExpressionArgument>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        argument_type,
        std::move(expression)
    );
}

std::unique_ptr<PredicateArgument> parse_predicate_argument(
    Compiler* compiler,
    Script* script,
    Tokenizer* tokens
) {
    auto predicate_script_name = compiler->generate_script_name("predicate", script, tokens->get_line_index());

    auto predicate_script_id = compiler->register_script_source(
        predicate_script_name,
        script->get_path(),
        tokens->get_line()
    );

    auto predicate_script = compiler->get_runtime()->get_script(predicate_script_id);

    auto predicate_expression = parser::parse_expression(compiler, script, tokens);

    // Wrap predicate in return statement.
    auto return_statement = std::make_unique<ReturnStatementNode>(tokens->get_line_index(), tokens->get_column_index(), std::move(predicate_expression));

    // Wrap statement in statement list.
    auto script_node = std::make_unique<ScriptNode>(tokens->get_line_index(), tokens->get_column_index());
    script_node->add_statement(std::move(return_statement));

    // Assign statement list to script
    compiler->set_script_node(predicate_script_id, std::move(script_node));

    auto predicate_argument = std::make_unique<PredicateArgument>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        predicate_script_id
    );

    return predicate_argument;
}

} // namespace parser

} // namespace nomad