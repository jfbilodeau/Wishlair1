//
// Created by jfbil on 2023-10-08.
//

#ifndef NOMAD_PARSER_HPP
#define NOMAD_PARSER_HPP

#include "nomad/Nomad.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/compiler/Argument.hpp"
#include "nomad/compiler/Expression.hpp"

#include "nomad/compiler/SyntaxTree.hpp"

namespace nomad {

// Forward declaration
class Compiler;
class Tokenizer;

using PreParseStatementFn = void (*)(Compiler*, Script*, Tokenizer*);
using ParseStatementFn = std::unique_ptr<StatementNode> (*)(Compiler*, Script*, Tokenizer*);

class ParserException: public NomadException {
public:
    explicit ParserException(const NomadString& message, NomadIndex row = 0, NomadIndex column = 0);

    [[nodiscard]] NomadIndex get_row() const { return m_row; }
    [[nodiscard]] NomadIndex get_column() const { return m_column; }

private:
    NomadIndex m_row;
    NomadIndex m_column;
};

namespace parser {

[[noreturn]] void throw_parser_error(const NomadString& message);
[[noreturn]] void throw_parser_error(const NomadString& message, Tokenizer* tokens);

void expect_end_of_line(Compiler* compiler, Script* script, Tokenizer* tokens);

IdentifierType get_identifier_type(Compiler* compiler, const NomadString& name, Script* script = nullptr);

std::unique_ptr<StatementNode> parse_line(Compiler* compiler, Script* script, Tokenizer* tokenizer);
void parse_block(Compiler* compiler, Script* script, Tokenizer* tokens, const std::vector<NomadString>& end_tokens, StatementList* statements);
std::unique_ptr<StringLiteral> parse_string_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parse_assignment_statement(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<StatementNode> parse_statement(Compiler* compiler, Script* script, Tokenizer* tokenizer);
std::unique_ptr<StatementNode> parse_command_statement(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command);
void parse_script_parameter(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandParameterDefinition& parameter);
std::unique_ptr<StatementNode> parse_script_call(Compiler* compiler, Script* script, Tokenizer* tokens, const NomadString& script_name);
std::unique_ptr<Expression> parse_logical_and_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_logical_or_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_relational_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_bitwise_and_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_bitwise_xor_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_bitwise_or_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_term_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_product_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_parentheses_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_unary_operator_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_primary_expression(Compiler* compiler, Script* script, Tokenizer* tokens);
std::unique_ptr<Expression> parse_script_call_expression(Compiler* compiler, Script* script, Tokenizer* tokens, const NomadString& script_name);
void parse_script_arguments(Compiler* compiler, Script* script, Tokenizer* tokens, Script* target_script, ArgumentList* arguments);
std::unique_ptr<Expression> parse_command_call_expression(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command);
void parse_command_arguments(Compiler* compiler, Script* script, Tokenizer* tokens, const CommandDefinition& command, ArgumentList* arguments);
std::unique_ptr<ExpressionArgument> parse_expression_argument(Compiler* compiler, const Type* argument_type, Script* script, Tokenizer* tokens);
std::unique_ptr<PredicateArgument> parse_predicate_argument(Compiler* compiler, Script* script, Tokenizer* tokens);

} // namespace parser

} // namespace nomad

#endif //NOMAD_PARSER_HPP
