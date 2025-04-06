//
// Created by jfbil on 2024-10-07.
//

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/FormatStringCompiler.hpp"

#include "nomad/compiler/Expression.hpp"
#include "nomad/compiler/SyntaxTree.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Expressions
Expression::Expression(NomadIndex line, NomadIndex column, const Type* type):
    AstNode(line, column),
    m_type(type)
{ }

Expression::~Expression() {
    if (m_type != nullptr) {
        m_type->freeValue(m_value);
    }
}

void Expression::parse(Compiler* compiler, Script* script) {
    if (!isParsed()) {
        onParse(compiler, script);
    }
}

void Expression::compile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (isParsed()) {
        compiler->addLoadValue(m_type, m_value);
    } else {
        onCompile(compiler, script);
    }
}

const Type* Expression::getType() const {
    return m_type;
}

const ScriptValue& Expression::getValue() const {
    return m_value;
}

bool Expression::hasValue() const {
    return m_hasValue;
}

bool Expression::hasType() const {
    return m_type != nullptr;
}

bool Expression::isParsed() const {
    return hasType() && hasValue();
}

void Expression::setResolved(const ScriptValue& value, const Type* type) {
    setType(type);
    setValue(value);
}

void Expression::setType(const Type* type) {
    m_type = type;
}

void Expression::setValue(const ScriptValue& value) {
    m_type->copyValue(value, m_value);

    m_hasValue = true;
}

///////////////////////////////////////////////////////////////////////////////
// UnaryExpression
UnaryExpression::UnaryExpression(
    NomadIndex line, NomadIndex column, UnaryOperator unaryOperator,
    std::unique_ptr<Expression> expression
):
    Expression(line, column),
    m_unaryOperator(unaryOperator),
    m_expression(std::move(expression)) {
}


void UnaryExpression::onParse(Compiler* compiler, Script* script) {
    m_expression->parse(compiler, script);

    if (m_expression->isParsed()) {
        auto expressionType = m_expression->getType();

        auto value = m_expression->getValue();
        ScriptValue result;

        auto resultType = compiler->getUnaryOperatorResultType(m_unaryOperator, expressionType);
        auto foldSuccess = compiler->foldUnary(m_unaryOperator, expressionType, value, result);

        if (!foldSuccess) {
            raiseException("Cannot fold unary operator");
        }

        setResolved(result, resultType);
    } else {
        setType(m_expression->getType());
    }
}

void UnaryExpression::onCompile(Compiler* compiler, Script* script) {
    onParse(compiler, script);

    m_expression->compile(compiler, script);

    auto opcode = compiler->getUnaryOperatorOpCodeId(m_unaryOperator, m_expression->getType());

    compiler->addOpCode(opcode);
}
///////////////////////////////////////////////////////////////////////////////
// BinaryExpression
BinaryExpression::BinaryExpression(NomadIndex line, NomadIndex column, BinaryOperator binaryOperator,
    std::unique_ptr<Expression> left, std::unique_ptr<Expression> right
):
    Expression(line, column),
    m_binaryOperator(binaryOperator),
    m_left(std::move(left)),
    m_right(std::move(right)) {
}

void BinaryExpression::onParse(Compiler* compiler, Script* script) {
    m_left->parse(compiler, script);
    m_right->parse(compiler, script);

    auto lhsType = m_left->getType();
    auto rhsType = m_right->getType();

    if (lhsType != nullptr && rhsType != nullptr) {

        auto resultType = compiler->getBinaryOperatorResultType(m_binaryOperator, lhsType, rhsType);

        setType(resultType);
    }

    if (m_left->isParsed() && m_right->isParsed()) {
        auto lhs = m_left->getValue();
        auto rhs = m_right->getValue();

        ScriptValue value;

        auto resultType = compiler->getBinaryOperatorResultType(m_binaryOperator, lhsType, rhsType);
        auto result = compiler->foldBinary(m_binaryOperator, lhsType, lhs, rhsType, rhs, value);

        if (!result) {
            raiseException("Cannot fold binary operator");
        }

        setResolved(value, resultType);
    }
}

void BinaryExpression::onCompile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (isParsed()) {
        compiler->addLoadValue(getType(), getValue());
        return;
    }

    m_right->compile(compiler, script);
    compiler->addPushResult(m_right->getType());
    m_left->compile(compiler, script);
    compiler->addPopIntermediate(m_left->getType());

    auto leftType = m_left->getType();
    auto rightType = m_right->getType();

    auto opCodeId = compiler->getBinaryOperatorOpCodeId(
        m_binaryOperator,
        leftType,
        rightType
    );

    if (opCodeId == NOMAD_INVALID_ID) {
        raiseException("Invalid binary operator");
    }

    compiler->addOpCode(opCodeId);
}

///////////////////////////////////////////////////////////////////////////////
// CallCommandExpression
CallCommandExpression::CallCommandExpression(NomadIndex line, NomadIndex column, NomadString name):
    Expression(line, column),
    m_name(std::move(name)) {
}

void CallCommandExpression::onParse(Compiler* compiler, Script* script) {
    auto commandId = compiler->getRuntime()->getCommandId(m_name);

    if (commandId == NOMAD_INVALID_ID) {
        raiseException("Unknown command '" + m_name + "'");
    }

    CommandDefinition commandDefinition;
    compiler->getRuntime()->getCommandDefinition(commandId, commandDefinition);

    setType(commandDefinition.returnType);
}

void CallCommandExpression::onCompile(Compiler* compiler, Script* script) {
    m_arguments.compile(compiler, script);

    auto commandId = compiler->getRuntime()->getCommandId(m_name);

    if (commandId == NOMAD_INVALID_ID) {
        raiseException("Unknown command '" + m_name + "'");
    }

    compiler->addCommandCall(commandId);
}

void CallCommandExpression::addArgument(std::unique_ptr<Argument> argument) {
    m_arguments.add(std::move(argument));
}

ArgumentList* CallCommandExpression::getArguments() {
    return &m_arguments;
}

///////////////////////////////////////////////////////////////////////////////
// CallScriptExpression
ScriptCallExpression::ScriptCallExpression(NomadIndex line, NomadIndex column, NomadString name):
    Expression(line, column),
    m_name(std::move(name)) {
}

void ScriptCallExpression::onParse(Compiler* compiler, Script* script) {
    auto callScriptId = compiler->getRuntime()->getScriptId(m_name);

    auto callScript = compiler->getRuntime()->getScript(callScriptId);

    setType(callScript->getReturnType());
}

void ScriptCallExpression::onCompile(Compiler* compiler, Script* script) {
    auto targetScriptId = compiler->getRuntime()->getScriptId(m_name);

    if (targetScriptId == NOMAD_INVALID_ID) {
        raiseException("Unknown script '" + m_name + "'");
    }

    compiler->addScriptCall(targetScriptId);
}

void ScriptCallExpression::addArgument(std::unique_ptr<Expression> argument) {
    m_arguments.push_back(std::move(argument));
}

NomadIndex ScriptCallExpression::getArgumentCount() const {
    return m_arguments.size();
}

///////////////////////////////////////////////////////////////////////////////
// PrimaryExpression
PrimaryExpression::PrimaryExpression(NomadIndex line, NomadIndex column):
    Expression(line, column) {
}

///////////////////////////////////////////////////////////////////////////////
// BooleanLiteral
BooleanLiteral::BooleanLiteral(NomadIndex line, NomadIndex column, NomadBoolean m_value):
    PrimaryExpression(line, column),
    m_value(m_value) {
}

void BooleanLiteral::onParse(Compiler* compiler, Script* script) {
    setResolved(ScriptValue{m_value}, compiler->getRuntime()->getBooleanType());
}

void BooleanLiteral::onCompile(Compiler* compiler, Script* script) {
    compiler->addLoadBooleanValue(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// IntegerExpression

IntegerLiteral::IntegerLiteral(NomadIndex line, NomadIndex column, NomadInteger value):
    PrimaryExpression(line, column),
    m_value(value) {
}

void IntegerLiteral::onParse(Compiler* compiler, Script* script) {
    ScriptValue value{m_value};

    setResolved(value, compiler->getRuntime()->getIntegerType());
}

void IntegerLiteral::onCompile(Compiler* compiler, Script* script) {
    compiler->addLoadIntegerValue(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// FloatLiteralExpression
FloatLiteral::FloatLiteral(NomadIndex line, NomadIndex column, NomadFloat value):
    PrimaryExpression(line, column),
    m_value(value) {
}

void FloatLiteral::onParse(Compiler* compiler, Script* script) {
    ScriptValue value{m_value};

    setResolved(value, compiler->getRuntime()->getFloatType());
}

void FloatLiteral::onCompile(Compiler* compiler, Script* script) {
    compiler->addLoadFloatValue(m_value);
}

///////////////////////////////////////////////////////////////////////////////
// StringLiteralExpression

StringLiteral::StringLiteral(NomadIndex line, NomadIndex column, NomadString value):
    PrimaryExpression(line, column),
    m_value(std::move(value)) {
}

void StringLiteral::onParse(Compiler* compiler, Script* script) {
    setResolved(ScriptValue{m_value}, compiler->getRuntime()->getStringType());
}

void StringLiteral::onCompile(Compiler* compiler, Script* script) {
    auto stringValue = getValue().getStringValue();

    auto stringId = compiler->getRuntime()->registerString(stringValue);

    compiler->addOpCode(OpCodes::op_string_load_r);
    compiler->addId(stringId);
}

///////////////////////////////////////////////////////////////////////////////
// ConstantValueExpression
ConstantValueExpression::ConstantValueExpression(NomadIndex line, NomadIndex column, const NomadString& constantName):
    PrimaryExpression(line, column),
    m_constantName(constantName) {
}

void ConstantValueExpression::onParse(Compiler* compiler, Script* script) {
    m_constantId = compiler->getRuntime()->getConstantId(m_constantName);

    if (m_constantId != NOMAD_INVALID_ID) {
        ScriptValue constantValue;
        compiler->getRuntime()->getConstantValue(m_constantId, constantValue);
        auto constantType = compiler->getRuntime()->getConstantType(m_constantId);

        setResolved(constantValue, constantType);
    }
}

void ConstantValueExpression::onCompile(Compiler* compiler, Script* script) {
    parse(compiler, script);

    if (!isParsed()) {
        raiseException("Unknown constant: " + m_constantName);
    }

    compiler->addLoadValue(getType(), getValue());
}

///////////////////////////////////////////////////////////////////////////////
// FormatStringLiteral
FormatStringLiteral::FormatStringLiteral(NomadIndex line, NomadIndex column, const NomadString& formatString):
    Expression(line, column),
    m_formatString(formatString) {
}

void FormatStringLiteral::onParse(Compiler* compiler, Script* script) {
    setType(compiler->getRuntime()->getStringType());
}

void FormatStringLiteral::onCompile(Compiler* compiler, Script* script) {
    NomadId scriptId = script->getId();

    auto formatStringId = compiler->getRuntime()->getFormatStringId(m_formatString, scriptId);

    if (formatStringId == NOMAD_INVALID_ID) {
        formatStringId = compiler->getRuntime()->registerFormatString(m_formatString, scriptId);

        auto format_string = compiler->getRuntime()->getFormatString(formatStringId);

        compileFormatString(compiler, script, m_formatString, format_string);
    }

    compiler->addOpCode(OpCodes::op_format_string_execute);
    compiler->addId(formatStringId);
}


IdentifierExpression::IdentifierExpression(NomadIndex line, NomadIndex column, const NomadString& identifier):
    PrimaryExpression(line, column),
    m_identifier(identifier)
{
}
void IdentifierExpression::onParse(nomad::Compiler* compiler, Script* script) {
    compiler->getIdentifierDefinition(m_identifier, script, m_identifierDefinition);

    // Check for invalid identifier type.
    switch (m_identifierDefinition.identifierType) {
        case IdentifierType::Unknown:
            // Unknown identifier type
            compiler->reportError("Unknown identifier: " + m_identifier);

        case IdentifierType::Script:
            compiler->reportError("Cannot use script as identifier: "+ m_identifier);

        case IdentifierType::Command:
            compiler->reportError("Cannot use command as identifier: "+ m_identifier);

        case IdentifierType::Keyword:
            compiler->reportError("Cannot use keyword as identifier: "+ m_identifier);

        case IdentifierType::Statement:
            compiler->reportError("Cannot use statement as identifier: "+ m_identifier);

        default:
            // Valid identifier type. Ignore
            break; 
    }

    if (m_identifierDefinition.identifierType == IdentifierType::ContextVariable) {
        auto contextId = compiler->getRuntime()->getVariableContextIdByPrefix(m_identifier);

        if (contextId == NOMAD_INVALID_ID) {
            compiler->reportError("Unknown context for identifier: " + m_identifier);
        }

        auto context = compiler->getRuntime()->getVariableContext(contextId);

        context->registerVariable(m_identifier, m_identifierDefinition.valueType);
    }

    if (m_identifierDefinition.identifierType == IdentifierType::ScriptVariable) {
        script->registerVariable(m_identifier, m_identifierDefinition.valueType);
    }

    setType(m_identifierDefinition.valueType);
}

void IdentifierExpression::onCompile(Compiler* compiler, Script* script) {
    switch (m_identifierDefinition.identifierType) {
        case IdentifierType::ScriptVariable:
            compiler->addOpCode(OpCodes::op_script_variable_get);
            compiler->addId(m_identifierDefinition.variableId);
            break;

        case IdentifierType::DynamicVariable:
            compiler->addOpCode(OpCodes::op_dynamic_variable_get);
            compiler->addId(m_identifierDefinition.variableId);
            break;

        case IdentifierType::ContextVariable: {
            auto contextId = compiler->getRuntime()->getVariableContextIdByPrefix(m_identifier);

            if (contextId == NOMAD_INVALID_ID) {
                compiler->reportError("Unknown context for identifier: " + m_identifier);
            }

            auto context = compiler->getRuntime()->getVariableContext(contextId);

            context->registerVariable(m_identifier, m_identifierDefinition.valueType);

            compiler->addOpCode(OpCodes::op_context_variable_get);
            compiler->addId(m_identifierDefinition.contextId);
            compiler->addId(m_identifierDefinition.variableId);
            break;
        }

        case IdentifierType::Parameter:
            if (m_identifierDefinition.valueType == nullptr) {
                compiler->reportError("Unknown type for parameter: " + m_identifier);
            }

            if (m_identifierDefinition.valueType == compiler->getRuntime()->getStringType()) {
                compiler->addOpCode(OpCodes::op_string_parameter_load_r);
            } else {
                compiler->addOpCode(OpCodes::op_parameter_load_r);
            }
            compiler->addId(m_identifierDefinition.variableId);
            break;

        default:
            raiseException("Invalid identifier type");
    }
}

} // namespace nomad