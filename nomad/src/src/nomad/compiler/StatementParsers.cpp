//
// Created by jfbil on 2023-06-05.
//

#include "nomad/compiler/StatementParsers.hpp"

#include "nomad/nomad.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Parser.hpp"
#include "nomad/compiler/Tokenizer.hpp"
#include "nomad/compiler/SyntaxTree.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/system/TempHeap.hpp"
#include "nomad/system/String.hpp"

#include <optional>

namespace nomad {

NomadString generate_fun_script_name(Compiler* compiler, Script* script, NomadIndex line) {
    // Generate script name
    NomadString test_script_name;
    auto count = 0;
    auto test_id = NOMAD_INVALID_ID;

    do {
        test_script_name = script->get_name() + "@" + to_string(line) + "[" + to_string(count) + "]";
        test_id = compiler->get_runtime()->get_script_id(test_script_name);
        count++;

        if (count > 1000) {
            // Hopefully this will never happen...
            compiler->report_error("Internal error: could not generate `fun` script name");
        }
    } while (test_id != NOMAD_INVALID_ID);

    return test_script_name;
}

FunStatementNode::FunStatementNode(NomadIndex line, NomadIndex column, Script* script, std::unique_ptr<Expression> ptr):
    StatementNode(line, column),
    m_script(script)
{}

void FunStatementNode::on_compile(Compiler* compiler, Script* script) {
    // `fun` will be build along other scripts
}

void FunStatementNode::add_parameter(const NomadString& parameter_name) {
    m_parameters.push_back(parameter_name);
}

NomadIndex FunStatementNode::get_parameter_count() const {
    return m_parameters.size();
}

Script* FunStatementNode::get_script() const {
    return m_script;
}

const NomadString& FunStatementNode::get_script_name() const {
    return m_script->get_name();
}

StatementList& FunStatementNode::get_body() {
    return m_body;
}

void pre_parse_fun_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& fun_token = tokens->next_token();
    auto& fun_name = fun_token.text_value;

    // Make sure name is not already in use.
    auto identifier_type = parser::get_identifier_type(compiler, fun_name);

    if (identifier_type != IdentifierType::Unknown) {
        parser::throw_parser_error("Function name '" + fun_name + "' is already used", tokens);
    }

    // Generate `fun`
    auto fun_id = compiler->register_script_source(
        fun_name,
        script->get_path(),
        script->get_source()
    );

    auto fun = compiler->get_runtime()->get_script(fun_id);

    // Parse parameters.
    pre_parse_params_statement(compiler, fun, tokens);
}
//
//void parse_fun_parameters(Compiler* compiler, Script* script, Tokenizer* tokens) {
//    while (!tokens->end_of_line()) {
//        NomadString parameter_name, parameter_type_name;
//
//        tokens->next_identifier(parameter_name);
//        tokens->next_identifier(parameter_type_name);
//
//        if (script->get_parameter_id(parameter_name) != NOMAD_INVALID_ID) {
//            parser::throw_parser_error("Parameter '" + parameter_name + "' is already defined", tokens);
//        }
//
//        auto paramenter_type = compiler->get_runtime()->get_type(parameter_type_name);
//
//        if (paramenter_type == nullptr) {
//            parser::throw_parser_error("Unknown type '" + parameter_type_name + "' for parameter", tokens);
//        }
//
//        script->add_parameter(parameter_name, paramenter_type);
//    }
//
//    tokens->next_line();
//}

void parse_callback_fun_parameters(Compiler* compiler, Script* script, Tokenizer* tokens, const ScriptType* callback_type) {
    for (auto i = 0; i < callback_type->get_parameter_count(); ++i) {
        NomadString parameter_name;
        tokens->next_identifier(parameter_name);

        auto parameter_type = callback_type->get_parameter_type(i);

        if (script->get_parameter_id(parameter_name) != NOMAD_INVALID_ID) {
            parser::throw_parser_error("Parameter '" + parameter_name + "' is already defined", tokens);
        }

        script->add_parameter(parameter_name, parameter_type);
    }
}

std::unique_ptr<ScriptNode> parse_fun_body(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto fun = std::make_unique<ScriptNode>(tokens->get_line_index(), 0);

    parser::parse_block(compiler, script, tokens, {"end"}, fun->get_statements());

    return fun;
}

std::unique_ptr<StatementNode> parse_fun_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    // Consume `fun` keyword
    auto& token = tokens->next_token();

    if (token.type != TokenType::Identifier) {
        parser::throw_parser_error("Identifier expected", tokens);
    }

    auto fun_name = token.text_value;

//    auto identifier_type = parser::get_identifier_type(compiler, fun_name);
//
//    if (identifier_type != IdentifierType::Unknown) {
//        parser::throw_parser_error("Function name '" + fun_name + "' is already used", tokens);
//    }

    // Create script
//    auto script_id = compiler->register_script_source(
//        fun_name,
//        script->get_path(),
//        script->get_source()
//    );
//
//    if (script_id == NOMAD_INVALID_ID) {
//        parser::throw_parser_error("Failed to register script '" + fun_name + "'", tokens);
//    }

    // Fun is already registered by pre_parse_fun_statement
    tokens->next_line();

    auto fun_id =  compiler->get_runtime()->get_script_id(fun_name);
    auto fun_script = compiler->get_runtime()->get_script(fun_id);

    if (fun_script == nullptr) {
        parser::throw_parser_error("[parse_fun_statement()] Internal error: Failed to get script '" + fun_name + "'", tokens);
    }

    // Parse parameters
//    parse_fun_parameters(compiler, script, tokens);

    auto fun_statement = parse_fun_body(compiler, fun_script, tokens);

    compiler->set_script_node(fun_id, std::move(fun_statement));

    return nullptr;
}

//class ReturnStatement : public StatementNode {
//public:
//    ReturnStatement(NomadIndex row, NomadIndex col, std::unique_ptr<Expression> expression) :
//        StatementNode(row, col),
//        m_expression(std::move(expression))
//    {}
//
//protected:
//    void on_compile(Compiler* compiler, Script* script) override {
//        m_expression->compile(compiler, script);
//
//        compiler->add_op_code(OpCodes::op_return);
//    }
//
//private:
//    std::unique_ptr<Expression> m_expression;
//};
//
//std::unique_ptr<StatementNode> parse_return_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
//    auto expression = parser::parse_expression(compiler, script, tokens);
//
//    return std::make_unique<ReturnStatement>(tokens->get_line_index(), tokens->get_column_index(), std::move(expression));
//}

class AssertStatement : public StatementNode {
public:
    AssertStatement(NomadIndex row, NomadIndex col, NomadString line, std::unique_ptr<Expression> expression) :
        StatementNode(row, col),
        m_line(std::move(line)),
        m_expression(std::move(expression))
    {}

protected:
    void on_compile(Compiler* compiler, Script* script) override {
        auto assert_message =
            "Assertion failed: " +
            script->get_name() +
            "[" +
            to_string(get_line() + 1) +
            "]: " +
            m_line;

        auto assert_message_id = compiler->get_runtime()->register_string(assert_message);

        m_expression->compile(compiler, script);

        compiler->add_op_code(OpCodes::op_id_load_i);
        compiler->add_id(assert_message_id);

        compiler->add_op_code(OpCodes::op_assert);
    }

private:
    NomadString m_line;
    std::unique_ptr<Expression> m_expression;
};

std::unique_ptr<StatementNode> parse_assert_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto line = string_trim_copy(tokens->get_line());

    auto expression = parser::parse_expression(compiler, script, tokens);

    return std::make_unique<AssertStatement>(
        tokens->get_line_index(),
        tokens->get_column_index(),
        std::move(line),
        std::move(expression)
    );
}

class ConstStatement : public StatementNode {
public:
    ConstStatement(NomadIndex row, NomadIndex col, NomadString name, const Type* type, const ScriptValue& value) :
        StatementNode(row, col),
        m_name(std::move(name)),
        m_type(type),
        m_value(value)
    {}

protected:
    void on_compile(Compiler* compiler, Script* script) override {
        // Nothing to do...
    }

private:
    NomadString m_name;
    const Type* m_type;
    ScriptValue m_value;
};

struct ConstantValue {
    ~ConstantValue() {
        if (type != nullptr) {
            type->free_value(value);
        }
    }

    ScriptValue value;
    const Type* type = nullptr;
};

void parse_constant_expression(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant);

void parse_constant_primary(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    auto& token = tokens->next_token();

    if (token.type == TokenType::Boolean) {
        constant.value.set_boolean_value(token.boolean_value);
        constant.type = compiler->get_runtime()->get_boolean_type();

        return;
    }

    if (token.type == TokenType::Integer) {
        constant.value.set_integer_value(token.integer_value);
        constant.type = compiler->get_runtime()->get_integer_type();

        return;
    }

    if (token.type == TokenType::Float) {
        constant.value.set_float_value(token.float_value);
        constant.type = compiler->get_runtime()->get_float_type();

        return;
    }

    if (token.type == TokenType::String) {
        constant.value.set_string_value(token.text_value);
        constant.type = compiler->get_runtime()->get_string_type();

        return;
    }

    // Constant reference?
    if (token.type == TokenType::Identifier) {
        auto constant_id = compiler->get_runtime()->get_constant_id(token.text_value);

        if (constant_id == NOMAD_INVALID_ID) {
            parser::throw_parser_error("Unknown constant '" + token.text_value + "'", tokens);
        }

        compiler->get_runtime()->get_constant_value(constant_id, constant.value);
        constant.type = compiler->get_runtime()->get_constant_type(constant_id);

        return;
    }

    parser::throw_parser_error("Unexpected token in constant expression: '" + token.text_value + "'", tokens);
}

void parse_constant_unary_operator(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    if (tokens->token_is("-") || tokens->token_is("+") || tokens->token_is("!")) {
        auto& token = tokens->next_token();

        UnaryOperator op;

        if (token.text_value == "-") {
            op = UnaryOperator::Minus;
        } else if (token.text_value == "+") {
            op = UnaryOperator::Plus;
        } else if (token.text_value == "!") {
            op = UnaryOperator::Bang;
        } else {
            parser::throw_parser_error("Unknown constant unary operator '" + token.text_value + "'", tokens);
        };

        parse_constant_primary(compiler, tokens, constant);

        auto fold_result = compiler->fold_unary(
            op,
            constant.type,
            constant.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold unary operator '" + token.text_value + "'", tokens);
        }

        return;
    }

    return parse_constant_primary(compiler, tokens, constant);
}

void parse_constant_parentheses(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {

    if (tokens->token_is("(")) {
        auto& token = tokens->next_token();

        parse_constant_expression(compiler, tokens, constant);

        if (!tokens->token_is(")")) {
            parser::throw_parser_error("Expected ')'", tokens);
        }

        tokens->next_token(); // Skip ')'

        return;
    }

    parse_constant_unary_operator(compiler, tokens, constant);
}

void parse_constant_product(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_primary(compiler, tokens, constant);

    if (tokens->token_is("*") || tokens->token_is("/") || tokens->token_is("%")) {
        auto& token = tokens->next_token();

        BinaryOperator op;

        if (token.text_value == "*") {
            op = BinaryOperator::Star;
        } else if (token.text_value == "/") {
            op = BinaryOperator::Slash;
        } else if (token.text_value == "%") {
            op = BinaryOperator::Percent;
        } else {
            parser::throw_parser_error("Unknown constant binary operator '" + token.text_value + "'", tokens);
        }

        ConstantValue rhs;
        parse_constant_product(compiler, tokens, rhs);

        bool fold_result = compiler->fold_binary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold binary operator '" + token.text_value + "'", tokens);
        }
    }
}

void parse_constant_term(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_product(compiler, tokens, constant);

    if (tokens->token_is("+") || tokens->token_is("-")) {
        auto& token = tokens->next_token();

        BinaryOperator op;

        if (token.text_value == "+") {
            op = BinaryOperator::Plus;
        } else if (token.text_value == "-") {
            op = BinaryOperator::Minus;
        } else {
            parser::throw_parser_error("Unknown constant binary operator '" + token.text_value + "'", tokens);
        }

        ConstantValue rhs;
        parse_constant_term(compiler, tokens, rhs);

        bool fold_result = compiler->fold_binary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold binary operator '" + token.text_value + "'", tokens);
        }
    }
}

void parse_constant_bitwise_or(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_term(compiler, tokens, constant);

    if (tokens->token_is("|")) {
        auto& token = tokens->next_token();

        BinaryOperator op;

        op = BinaryOperator::Pipe;

        ConstantValue rhs;
        parse_constant_bitwise_or(compiler, tokens, rhs);

        bool fold_result = compiler->fold_binary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold binary operator '" + token.text_value + "'", tokens);
        }
    }
}

void parse_constant_bitwise_xor(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_or(compiler, tokens, constant);

    if (tokens->token_is("^")) {
        auto& token = tokens->next_token();

        BinaryOperator op;

        op = BinaryOperator::Caret;

        ConstantValue rhs;
        parse_constant_bitwise_xor(compiler, tokens, rhs);

        bool fold_result = compiler->fold_binary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold binary operator '" + token.text_value + "'", tokens);
        }
    }
}


void parse_constant_bitwise_and(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_xor(compiler, tokens, constant);

    if (tokens->token_is("&")) {
        auto& token = tokens->next_token();

        BinaryOperator op;

        op = BinaryOperator::And;

        ConstantValue rhs;
        parse_constant_bitwise_and(compiler, tokens, rhs);

        bool fold_result = compiler->fold_binary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throw_parser_error("Failed to fold binary operator '" + token.text_value + "'", tokens);
        }
    }
}

void parse_constant_expression(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_and(compiler, tokens, constant);

    if (!tokens->end_of_line()) {
        parser::throw_parser_error("Unexpected token in constant expression: '" + tokens->current_token().text_value + "'", tokens);
    }
}

void pre_parse_const_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& constant_token = tokens->next_token();
    auto& constant_name = constant_token.text_value;

    if (constant_token.type != TokenType::Identifier) {
        parser::throw_parser_error("Identifier expected. Got '" + constant_name + "' instead", tokens);
    }

    tokens->expect(TokenType::Operator);
    tokens->expect("=");

    auto constant_id = compiler->get_runtime()->get_constant_id(constant_name);

    if (constant_id != NOMAD_INVALID_ID) {
        parser::throw_parser_error("Constant '" + constant_name + "' is already defined", tokens);
    }

    ConstantValue constant;
    parse_constant_expression(compiler, tokens, constant);

    compiler->get_runtime()->register_constant(constant_name, constant.value, constant.type);
}

class IfStatementNode : public StatementNode {
private:
    struct Branch {
        std::unique_ptr<Expression> expression;
        StatementList statements;
    };

public:
    IfStatementNode(NomadIndex col, NomadIndex row) :
        StatementNode(col, row)
    {
    }

    void set_expression(std::unique_ptr<Expression> expression) {
        m_expression = std::move(expression);
    }

    StatementList* get_body() { return &m_body; }

    void add_branch(std::unique_ptr<Expression> expression, StatementList statements) {
        auto branch = Branch {
            .expression = std::move(expression),
            .statements = std::move(statements)
        };

        m_branches.push_back(std::move(branch));
    }

    void set_else_statements(StatementList statements) {
        m_else_statements = std::move(statements);
    }

protected:
    void on_compile(Compiler* compiler, Script* script) override {
        std::vector<size_t> jump_end_indices;

        m_expression->compile(compiler, script);

        compiler->add_op_code(OpCodes::op_jump_if_false);
        auto if_jump_instruction_index = compiler->add_index(NOMAD_INVALID_INDEX); // Placeholder for jump address.

        m_body.compile(compiler, script);

        if (!m_branches.empty() || !m_else_statements.is_empty()) {
            // Jump to end...
            compiler->add_op_code(OpCodes::op_jump);

            auto if_jump_index = compiler->add_index(NOMAD_INVALID_INDEX); // Placeholder for jump

            jump_end_indices.push_back(if_jump_index);
        }

        compiler->set_index(if_jump_instruction_index, compiler->get_op_code_size());

        for (auto branch = m_branches.begin(); branch != m_branches.end(); ++branch) {
            branch->expression->compile(compiler, script);

            compiler->add_op_code(OpCodes::op_jump_if_false);
            auto else_if_jump_instruction_index = compiler->add_index(NOMAD_INVALID_INDEX); // Placeholder for jump address.

            branch->statements.compile(compiler, script);

            // Jump to end...
            if (branch + 1 != m_branches.end() || !m_else_statements.is_empty()) {
                compiler->add_op_code(OpCodes::op_jump);
                auto else_if_jump_index = compiler->add_index(NOMAD_INVALID_INDEX); // Placeholder for jump

                jump_end_indices.push_back(else_if_jump_index);
            }

            compiler->set_index(else_if_jump_instruction_index, compiler->get_op_code_size());
        }

        if (m_else_statements.get_statement_count() > 0) {
            m_else_statements.compile(compiler, script);
        }

        // Resolve jump addresses to end of if statement
        for (auto index: jump_end_indices) {
            compiler->set_index(index, compiler->get_op_code_size());
        }
    }

private:
    std::unique_ptr<Expression> m_expression;
    StatementList m_body;
    std::vector<Branch> m_branches;
    StatementList m_else_statements;
};

std::unique_ptr<StatementNode>  parse_if_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto if_statement = std::make_unique<IfStatementNode>(tokens->get_column_index(), tokens->get_line_index());

    auto if_expression = parser::parse_expression(compiler, script, tokens);

    if_statement->set_expression(std::move(if_expression));

    parser::expect_end_of_line(compiler, script, tokens);
    tokens->next_line();

    parser::parse_block(compiler, script, tokens, {"elseIf", "else", "endIf"}, if_statement->get_body());

    if (string_trim_copy(tokens->get_line()) == "elseIf") {
        // Skip over 'elseIf'
        tokens->next_token();

        auto branch_expression = parser::parse_expression(compiler, script, tokens);
        auto branch_statements = StatementList();

        parser::expect_end_of_line(compiler, script, tokens);

        parser::parse_block(compiler, script, tokens, {"elseIf", "else", "endIf"}, &branch_statements);

        if_statement->add_branch(std::move(branch_expression), std::move(branch_statements));
    }

    if (string_trim_copy(tokens->get_line()) == "else") {
        // Make sure there's nothing else after `else`
        tokens->next_token();
        parser::expect_end_of_line(compiler, script, tokens);

        // Skip over 'else'
        tokens->next_line();

        StatementList else_statements;

        parser::parse_block(compiler, script, tokens, {"endIf"}, &else_statements);

        if_statement->set_else_statements(std::move(else_statements));
    }

    if (string_trim_copy(tokens->get_line()) != "endIf") {
        parser::throw_parser_error("Expected 'endIf' to close 'if' statement.", tokens);
    }

    return if_statement;
}

void pre_parse_params_statement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (script->get_parameter_count() > 0) {
        parser::throw_parser_error("Script '" + script->get_name() + "' already has parameters", tokens);
    }

    while (!tokens->end_of_line()) {
        NomadString parameter_name, parameter_type_name;

        parameter_name = tokens->next_identifier();
        tokens->expect(":");
        parameter_type_name = tokens->next_identifier();

        if (script->get_parameter_id(parameter_name) != NOMAD_INVALID_ID) {
            parser::throw_parser_error("Parameter '" + parameter_name + "' is already defined", tokens);
        }

        auto parameter_type = compiler->get_runtime()->get_type(parameter_type_name);

        if (parameter_type == nullptr) {
            parser::throw_parser_error("Unknown type '" + parameter_type_name + "' for parameter", tokens);
        }

        script->add_parameter(parameter_name, parameter_type);
    }
}

} // nomad