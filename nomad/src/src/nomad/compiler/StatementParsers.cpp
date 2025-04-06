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

NomadString generateFunScriptName(Compiler* compiler, Script* script, NomadIndex line) {
    // Generate script name
    NomadString test_script_name;
    auto count = 0;
    auto test_id = NOMAD_INVALID_ID;

    do {
        test_script_name = script->getName() + "@" + toString(line) + "[" + toString(count) + "]";
        test_id = compiler->getRuntime()->getScriptId(test_script_name);
        count++;

        if (count > 1000) {
            // Hopefully this will never happen...
            compiler->reportError("Internal error: could not generate `fun` script name");
        }
    } while (test_id != NOMAD_INVALID_ID);

    return test_script_name;
}

FunStatementNode::FunStatementNode(NomadIndex line, NomadIndex column, Script* script, std::unique_ptr<Expression> ptr):
    StatementNode(line, column),
    m_script(script)
{}

void FunStatementNode::onCompile(Compiler* compiler, Script* script) {
    // `fun` will be build along other scripts
}

void FunStatementNode::addParameter(const NomadString& parameter_name) {
    m_parameters.push_back(parameter_name);
}

NomadIndex FunStatementNode::getParameterCount() const {
    return m_parameters.size();
}

Script* FunStatementNode::getScript() const {
    return m_script;
}

const NomadString& FunStatementNode::getScriptName() const {
    return m_script->getName();
}

StatementList& FunStatementNode::getBody() {
    return m_body;
}

void preParseFunStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& fun_token = tokens->nextToken();
    auto& fun_name = fun_token.textValue;

    // Make sure name is not already in use.
    auto identifier_type = parser::getIdentifierType(compiler, fun_name);

    if (identifier_type != IdentifierType::Unknown) {
        parser::throwParserError("Function name '" + fun_name + "' is already used", tokens);
    }

    // Generate `fun`
    auto fun_id = compiler->registerScriptSource(
        fun_name,
        script->getPath(),
        script->getSource()
    );

    auto fun = compiler->getRuntime()->getScript(fun_id);

    // Parse parameters.
    preParseParamsStatement(compiler, fun, tokens);
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

void parseCallbackFunParameters(Compiler* compiler, Script* script, Tokenizer* tokens, const ScriptType* callbackType) {
    for (auto i = 0; i < callbackType->get_parameter_count(); ++i) {
        NomadString parameter_name;
        tokens->nextIdentifier(parameter_name);

        auto parameter_type = callbackType->get_parameter_type(i);

        if (script->getParameterId(parameter_name) != NOMAD_INVALID_ID) {
            parser::throwParserError("Parameter '" + parameter_name + "' is already defined", tokens);
        }

        script->add_parameter(parameter_name, parameter_type);
    }
}

std::unique_ptr<ScriptNode> parseFunBody(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto fun = std::make_unique<ScriptNode>(tokens->getLineIndex(), 0);

    parser::parseBlock(compiler, script, tokens, {"end"}, fun->getStatements());

    return fun;
}

std::unique_ptr<StatementNode> parseFunStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    // Consume `fun` keyword
    auto& token = tokens->nextToken();

    if (token.type != TokenType::Identifier) {
        parser::throwParserError("Identifier expected", tokens);
    }

    auto fun_name = token.textValue;

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
    tokens->nextLine();

    auto fun_id =  compiler->getRuntime()->getScriptId(fun_name);
    auto fun_script = compiler->getRuntime()->getScript(fun_id);

    if (fun_script == nullptr) {
        parser::throwParserError("[parse_fun_statement()] Internal error: Failed to get script '" + fun_name + "'", tokens);
    }

    // Parse parameters
//    parse_fun_parameters(compiler, script, tokens);

    auto fun_statement = parseFunBody(compiler, fun_script, tokens);

    compiler->setScriptNode(fun_id, std::move(fun_statement));

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
    void onCompile(Compiler* compiler, Script* script) override {
        auto assert_message =
            "Assertion failed: " +
            script->getName() +
            "[" +
            toString(getLine() + 1) +
            "]: " +
            m_line;

        auto assert_message_id = compiler->getRuntime()->registerString(assert_message);

        m_expression->compile(compiler, script);

        compiler->addOpCode(OpCodes::op_id_load_i);
        compiler->addId(assert_message_id);

        compiler->addOpCode(OpCodes::op_assert);
    }

private:
    NomadString m_line;
    std::unique_ptr<Expression> m_expression;
};

std::unique_ptr<StatementNode> parseAssertStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto line = stringTrimCopy(tokens->getLine());

    auto expression = parser::parseExpression(compiler, script, tokens);

    return std::make_unique<AssertStatement>(
        tokens->getLineIndex(),
        tokens->getColumnIndex(),
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
    void onCompile(Compiler* compiler, Script* script) override {
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
            type->freeValue(value);
        }
    }

    ScriptValue value;
    const Type* type = nullptr;
};

void parse_constant_expression(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant);

void parse_constant_primary(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    auto& token = tokens->nextToken();

    if (token.type == TokenType::Boolean) {
        constant.value.setBooleanValue(token.booleanValue);
        constant.type = compiler->getRuntime()->getBooleanType();

        return;
    }

    if (token.type == TokenType::Integer) {
        constant.value.setIntegerValue(token.integerValue);
        constant.type = compiler->getRuntime()->getIntegerType();

        return;
    }

    if (token.type == TokenType::Float) {
        constant.value.setFloatValue(token.floatValue);
        constant.type = compiler->getRuntime()->getFloatType();

        return;
    }

    if (token.type == TokenType::String) {
        constant.value.setStringValue(token.textValue);
        constant.type = compiler->getRuntime()->getStringType();

        return;
    }

    // Constant reference?
    if (token.type == TokenType::Identifier) {
        auto constant_id = compiler->getRuntime()->getConstantId(token.textValue);

        if (constant_id == NOMAD_INVALID_ID) {
            parser::throwParserError("Unknown constant '" + token.textValue + "'", tokens);
        }

        compiler->getRuntime()->getConstantValue(constant_id, constant.value);
        constant.type = compiler->getRuntime()->getConstantType(constant_id);

        return;
    }

    parser::throwParserError("Unexpected token in constant expression: '" + token.textValue + "'", tokens);
}

void parse_constant_unary_operator(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    if (tokens->tokenIs("-") || tokens->tokenIs("+") || tokens->tokenIs("!")) {
        auto& token = tokens->nextToken();

        UnaryOperator op;

        if (token.textValue == "-") {
            op = UnaryOperator::Minus;
        } else if (token.textValue == "+") {
            op = UnaryOperator::Plus;
        } else if (token.textValue == "!") {
            op = UnaryOperator::Bang;
        } else {
            parser::throwParserError("Unknown constant unary operator '" + token.textValue + "'", tokens);
        };

        parse_constant_primary(compiler, tokens, constant);

        auto fold_result = compiler->foldUnary(
            op,
            constant.type,
            constant.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold unary operator '" + token.textValue + "'", tokens);
        }

        return;
    }

    return parse_constant_primary(compiler, tokens, constant);
}

void parse_constant_parentheses(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {

    if (tokens->tokenIs("(")) {
        auto& token = tokens->nextToken();

        parse_constant_expression(compiler, tokens, constant);

        if (!tokens->tokenIs(")")) {
            parser::throwParserError("Expected ')'", tokens);
        }

        tokens->nextToken(); // Skip ')'

        return;
    }

    parse_constant_unary_operator(compiler, tokens, constant);
}

void parse_constant_product(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_primary(compiler, tokens, constant);

    if (tokens->tokenIs("*") || tokens->tokenIs("/") || tokens->tokenIs("%")) {
        auto& token = tokens->nextToken();

        BinaryOperator op;

        if (token.textValue == "*") {
            op = BinaryOperator::Star;
        } else if (token.textValue == "/") {
            op = BinaryOperator::Slash;
        } else if (token.textValue == "%") {
            op = BinaryOperator::Percent;
        } else {
            parser::throwParserError("Unknown constant binary operator '" + token.textValue + "'", tokens);
        }

        ConstantValue rhs;
        parse_constant_product(compiler, tokens, rhs);

        bool fold_result = compiler->foldBinary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold binary operator '" + token.textValue + "'", tokens);
        }
    }
}

void parse_constant_term(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_product(compiler, tokens, constant);

    if (tokens->tokenIs("+") || tokens->tokenIs("-")) {
        auto& token = tokens->nextToken();

        BinaryOperator op;

        if (token.textValue == "+") {
            op = BinaryOperator::Plus;
        } else if (token.textValue == "-") {
            op = BinaryOperator::Minus;
        } else {
            parser::throwParserError("Unknown constant binary operator '" + token.textValue + "'", tokens);
        }

        ConstantValue rhs;
        parse_constant_term(compiler, tokens, rhs);

        bool fold_result = compiler->foldBinary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold binary operator '" + token.textValue + "'", tokens);
        }
    }
}

void parse_constant_bitwise_or(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_term(compiler, tokens, constant);

    if (tokens->tokenIs("|")) {
        auto& token = tokens->nextToken();

        BinaryOperator op;

        op = BinaryOperator::Pipe;

        ConstantValue rhs;
        parse_constant_bitwise_or(compiler, tokens, rhs);

        bool fold_result = compiler->foldBinary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold binary operator '" + token.textValue + "'", tokens);
        }
    }
}

void parse_constant_bitwise_xor(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_or(compiler, tokens, constant);

    if (tokens->tokenIs("^")) {
        auto& token = tokens->nextToken();

        BinaryOperator op;

        op = BinaryOperator::Caret;

        ConstantValue rhs;
        parse_constant_bitwise_xor(compiler, tokens, rhs);

        bool fold_result = compiler->foldBinary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold binary operator '" + token.textValue + "'", tokens);
        }
    }
}


void parse_constant_bitwise_and(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_xor(compiler, tokens, constant);

    if (tokens->tokenIs("&")) {
        auto& token = tokens->nextToken();

        BinaryOperator op;

        op = BinaryOperator::And;

        ConstantValue rhs;
        parse_constant_bitwise_and(compiler, tokens, rhs);

        bool fold_result = compiler->foldBinary(
            op,
            constant.type,
            constant.value,
            rhs.type,
            rhs.value,
            constant.value
        );

        if (!fold_result) {
            parser::throwParserError("Failed to fold binary operator '" + token.textValue + "'", tokens);
        }
    }
}

void parse_constant_expression(Compiler* compiler, Tokenizer* tokens, ConstantValue& constant) {
    parse_constant_bitwise_and(compiler, tokens, constant);

    if (!tokens->endOfLine()) {
        parser::throwParserError("Unexpected token in constant expression: '" + tokens->currentToken().textValue + "'", tokens);
    }
}

void preParseConstStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto& constant_token = tokens->nextToken();
    auto& constant_name = constant_token.textValue;

    if (constant_token.type != TokenType::Identifier) {
        parser::throwParserError("Identifier expected. Got '" + constant_name + "' instead", tokens);
    }

    tokens->expect(TokenType::Operator);
    tokens->expect("=");

    auto constant_id = compiler->getRuntime()->getConstantId(constant_name);

    if (constant_id != NOMAD_INVALID_ID) {
        parser::throwParserError("Constant '" + constant_name + "' is already defined", tokens);
    }

    ConstantValue constant;
    parse_constant_expression(compiler, tokens, constant);

    compiler->getRuntime()->registerConstant(constant_name, constant.value, constant.type);
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
    void onCompile(Compiler* compiler, Script* script) override {
        std::vector<size_t> jump_end_indices;

        m_expression->compile(compiler, script);

        compiler->addOpCode(OpCodes::op_jump_if_false);
        auto if_jump_instruction_index = compiler->addIndex(NOMAD_INVALID_INDEX); // Placeholder for jump address.

        m_body.compile(compiler, script);

        if (!m_branches.empty() || !m_else_statements.isEmpty()) {
            // Jump to end...
            compiler->addOpCode(OpCodes::op_jump);

            auto if_jump_index = compiler->addIndex(NOMAD_INVALID_INDEX); // Placeholder for jump

            jump_end_indices.push_back(if_jump_index);
        }

        compiler->setIndex(if_jump_instruction_index, compiler->getOpCodeSize());

        for (auto branch = m_branches.begin(); branch != m_branches.end(); ++branch) {
            branch->expression->compile(compiler, script);

            compiler->addOpCode(OpCodes::op_jump_if_false);
            auto else_if_jump_instruction_index = compiler->addIndex(NOMAD_INVALID_INDEX); // Placeholder for jump address.

            branch->statements.compile(compiler, script);

            // Jump to end...
            if (branch + 1 != m_branches.end() || !m_else_statements.isEmpty()) {
                compiler->addOpCode(OpCodes::op_jump);
                auto else_if_jump_index = compiler->addIndex(NOMAD_INVALID_INDEX); // Placeholder for jump

                jump_end_indices.push_back(else_if_jump_index);
            }

            compiler->setIndex(else_if_jump_instruction_index, compiler->getOpCodeSize());
        }

        if (m_else_statements.getStatementCount() > 0) {
            m_else_statements.compile(compiler, script);
        }

        // Resolve jump addresses to end of if statement
        for (auto index: jump_end_indices) {
            compiler->setIndex(index, compiler->getOpCodeSize());
        }
    }

private:
    std::unique_ptr<Expression> m_expression;
    StatementList m_body;
    std::vector<Branch> m_branches;
    StatementList m_else_statements;
};

std::unique_ptr<StatementNode>  parseIfStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    auto if_statement = std::make_unique<IfStatementNode>(tokens->getColumnIndex(), tokens->getLineIndex());

    auto if_expression = parser::parseExpression(compiler, script, tokens);

    if_statement->set_expression(std::move(if_expression));

    parser::expectEndOfLine(compiler, script, tokens);
    tokens->nextLine();

    parser::parseBlock(compiler, script, tokens, {"elseIf", "else", "endIf"}, if_statement->get_body());

    if (stringTrimCopy(tokens->getLine()) == "elseIf") {
        // Skip over 'elseIf'
        tokens->nextToken();

        auto branch_expression = parser::parseExpression(compiler, script, tokens);
        auto branch_statements = StatementList();

        parser::expectEndOfLine(compiler, script, tokens);

        parser::parseBlock(compiler, script, tokens, {"elseIf", "else", "endIf"}, &branch_statements);

        if_statement->add_branch(std::move(branch_expression), std::move(branch_statements));
    }

    if (stringTrimCopy(tokens->getLine()) == "else") {
        // Make sure there's nothing else after `else`
        tokens->nextToken();
        parser::expectEndOfLine(compiler, script, tokens);

        // Skip over 'else'
        tokens->nextLine();

        StatementList else_statements;

        parser::parseBlock(compiler, script, tokens, {"endIf"}, &else_statements);

        if_statement->set_else_statements(std::move(else_statements));
    }

    if (stringTrimCopy(tokens->getLine()) != "endIf") {
        parser::throwParserError("Expected 'endIf' to close 'if' statement.", tokens);
    }

    return if_statement;
}

void preParseParamsStatement(Compiler* compiler, Script* script, Tokenizer* tokens) {
    if (script->getParameterCount() > 0) {
        parser::throwParserError("Script '" + script->getName() + "' already has parameters", tokens);
    }

    while (!tokens->endOfLine()) {
        NomadString parameter_name, parameter_type_name;

        parameter_name = tokens->nextIdentifier();
        tokens->expect(":");
        parameter_type_name = tokens->nextIdentifier();

        if (script->getParameterId(parameter_name) != NOMAD_INVALID_ID) {
            parser::throwParserError("Parameter '" + parameter_name + "' is already defined", tokens);
        }

        auto parameter_type = compiler->getRuntime()->getType(parameter_type_name);

        if (parameter_type == nullptr) {
            parser::throwParserError("Unknown type '" + parameter_type_name + "' for parameter", tokens);
        }

        script->add_parameter(parameter_name, parameter_type);
    }
}

} // nomad