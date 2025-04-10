//
// Created by jfbil on 2023-06-04.
//

#include "nomad/compiler/FormatStringCompiler.hpp"
#include "nomad/compiler/Tokenizer.hpp"
#include "nomad/compiler/StatementParsers.hpp"

#include "nomad/compiler/statements/ReturnStatement.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/system/String.hpp"

#include "nomad/compiler/Compiler.hpp"

#include <filesystem>
#include <fstream>

namespace nomad {

// Utility to concatenate directory / filename
NomadString concatPath(const NomadString& directory, const NomadString& filename) {
    if (filename.empty() && directory.empty()) {
        // I feel like this should be an error...
        return "";
    }

    if (filename.empty()) {
        return directory;
    }

    if (directory.empty()) {
        return filename;
    }

    auto lastChar = directory.back();

    if (lastChar == '/' || lastChar == '\\') {
        return directory + filename;
    }

    return directory + '/' + filename;
}

Compiler::Compiler(Runtime *runtime, std::vector<Instruction>& instructions):
    m_runtime(runtime),
    m_instructions(instructions)
{
    if (m_instructions.empty()) {
        // Add op_stop instruction so that the return of a script is a stop instruction
        addOpCode(OpCodes::op_stop);
    }

    registerParseStatementFn("const", nullptr, preParseConstStatement);
    registerParseStatementFn("fun", parseFunStatement, preParseFunStatement);
    registerParseStatementFn("assert", parseAssertStatement);
    registerParseStatementFn("if", parseIfStatement);
    registerParseStatementFn("params", nullptr, preParseParamsStatement);
    registerParseStatementFn("return", parseReturnStatement);

    auto typeBoolean = runtime->getBooleanType();
    auto typeInteger = runtime->getIntegerType();
    auto typeFloat = runtime->getFloatType();
    auto typeString = runtime->getStringType();

    registerUnaryOperator(UnaryOperator::Bang, typeBoolean, typeBoolean, OpCodes::op_boolean_not, foldBooleanUnaryBang);

    registerUnaryOperator(UnaryOperator::Plus, typeInteger,  typeInteger, OpCodes::op_integer_absolute, foldIntegerUnaryPlus);
    registerUnaryOperator(UnaryOperator::Minus, typeInteger, typeInteger, OpCodes::op_integer_negate, foldIntegerUnaryMinus);

    registerUnaryOperator(UnaryOperator::Plus, typeFloat, typeFloat, OpCodes::op_float_absolute, foldFloatUnaryPlus);
    registerUnaryOperator(UnaryOperator::Minus, typeFloat, typeFloat, OpCodes::op_float_negate, foldFloatUnaryMinus);
    registerUnaryOperator(UnaryOperator::Sin, typeFloat, typeFloat, OpCodes::op_float_sin, foldFloatUnarySin);
    registerUnaryOperator(UnaryOperator::Cos, typeFloat, typeFloat, OpCodes::op_float_cosine, foldFloatUnaryCos);
    registerUnaryOperator(UnaryOperator::Tan, typeFloat, typeFloat, OpCodes::op_float_tangent, foldFloatUnaryTan);

    registerBinaryOperator(BinaryOperator::EqualEqual, typeBoolean, typeBoolean, typeBoolean, OpCodes::op_boolean_equal, foldBooleanBinaryAndAnd);
    registerBinaryOperator(BinaryOperator::BangEqual, typeBoolean, typeBoolean, typeBoolean, OpCodes::op_boolean_not_equal, foldBooleanBinaryPipePipe);

    registerBinaryOperator(BinaryOperator::Plus, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_add, foldIntegerBinaryPlus);
    registerBinaryOperator(BinaryOperator::Minus, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_subtract, foldIntegerBinaryMinus);
    registerBinaryOperator(BinaryOperator::Star, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_multiply, foldIntegerBinaryStar);
    registerBinaryOperator(BinaryOperator::Slash, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_divide, foldIntegerBinarySlash);
    registerBinaryOperator(BinaryOperator::Percent, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_modulo, foldIntegerBinaryPercent);
    registerBinaryOperator(BinaryOperator::Caret, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_xor, foldIntegerBinaryCaret);
    registerBinaryOperator(BinaryOperator::And, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_and, foldIntegerAnd);
    registerBinaryOperator(BinaryOperator::Pipe, typeInteger, typeInteger, typeInteger, OpCodes::op_integer_or, foldIntegerPipe);
    registerBinaryOperator(BinaryOperator::EqualEqual, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_equal, foldIntegerBinaryEqualEqual);
    registerBinaryOperator(BinaryOperator::BangEqual, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_not_equal, foldIntegerBinaryBangEqual);
    registerBinaryOperator(BinaryOperator::LessThan, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_less_than, foldIntegerBinaryLessThan);
    registerBinaryOperator(BinaryOperator::LessThanEqual, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_less_than_or_equal, foldIntegerBinaryLessThanEqual);
    registerBinaryOperator(BinaryOperator::GreaterThan, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_greater_than, foldIntegerBinaryGreaterThan);
    registerBinaryOperator(BinaryOperator::GreaterThanEqual, typeInteger, typeInteger, typeBoolean, OpCodes::op_integer_greater_than_or_equal, foldIntegerBinaryGreaterThanEqual);

    registerBinaryOperator(BinaryOperator::Plus, typeFloat, typeFloat, typeFloat, OpCodes::op_float_add, foldFloatBinaryPlus);
    registerBinaryOperator(BinaryOperator::Minus, typeFloat, typeFloat, typeFloat, OpCodes::op_float_subtract, foldFloatBinaryMinus);
    registerBinaryOperator(BinaryOperator::Star, typeFloat, typeFloat, typeFloat, OpCodes::op_float_multiply, foldFloatBinaryStar);
    registerBinaryOperator(BinaryOperator::Slash, typeFloat, typeFloat, typeFloat, OpCodes::op_float_divide, foldFloatBinarySlash);
    registerBinaryOperator(BinaryOperator::EqualEqual, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_equal, foldBooleanBinaryAndAnd);
    registerBinaryOperator(BinaryOperator::BangEqual, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_not_equal, foldBooleanBinaryPipePipe);
    registerBinaryOperator(BinaryOperator::LessThan, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_less_than, foldBooleanBinaryPipePipe);
    registerBinaryOperator(BinaryOperator::LessThanEqual, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_less_than_or_equal, foldBooleanBinaryPipePipe);
    registerBinaryOperator(BinaryOperator::GreaterThan, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_greater_than, foldBooleanBinaryPipePipe);
    registerBinaryOperator(BinaryOperator::GreaterThanEqual, typeFloat, typeFloat, typeBoolean, OpCodes::op_float_greater_than_or_equal, foldBooleanBinaryPipePipe);
}

Compiler::~Compiler() = default;

Runtime* Compiler::getRuntime() const {
    return m_runtime;
}

void Compiler::reportError(const NomadString& message) {
    throw CompilerException(message);
}

void Compiler::reportError(const NomadString& message, Script* script, Tokenizer* tokenizer) {
    reportError(message, script->getName(), tokenizer->getLineIndex(), tokenizer->getColumnIndex());
}

void Compiler::reportError(const NomadString& message, const NomadString& scriptName, NomadIndex line, NomadIndex column) {
    auto errorMessage = message + ": '" + scriptName + "' at line " + toString(line) + ", column " + toString(column);

    reportError(errorMessage);
}

void Compiler::reportInternalError(const NomadString& message) {
    throw CompilerException("Internal error: " + message);
}

void Compiler::registerParseStatementFn(const NomadString& name, ParseStatementFn fn, PreParseStatementFn preFn) {
    m_statements.emplace(name, ParseStatementFnRegistration{ preFn, fn });
}

bool Compiler::isStatement(const NomadString& name) const {
    return m_statements.find(name) != m_statements.end();
}

bool Compiler::getParseStatementFn(const NomadString& name, ParseStatementFn& fn) const {
    auto it = m_statements.find(name);

    if (it != m_statements.end()) {
        fn = it->second.fn;

        return true;
    }

    return false;
}

bool Compiler::getGetPreParseStatementFn(const NomadString& name, PreParseStatementFn& fn) const {
    auto it = m_statements.find(name);

    if (it != m_statements.end()) {
        fn = it->second.preFn;

        return true;
    }

    return false;
}

void Compiler::getRegisteredStatements(std::vector<NomadString>& parsers) const {
    for (auto& statement: m_statements) {
        parsers.push_back(statement.first);
    }
}

void Compiler::registerUnaryOperator(
    UnaryOperator op,
    const Type* operand,
    const Type* result,
    const NomadString& opCodeName,
    UnaryFoldingFn fn
) {
    // Sanity check
    if (m_runtime->getInstructionId(opCodeName) == NOMAD_INVALID_ID) {
        reportInternalError("Unknown op code '" + opCodeName + "'");
    }

    // Make sure the operator with the same and type doesn't already exist
    for (auto& definition: m_unaryOperators) {
        if (definition.op == op && definition.operand == operand) {
            reportInternalError("Unary operator '" + opCodeName + "' already registered for type '" + operand->getName() + "'");
        }
    }

    auto opCodeId = m_runtime->getInstructionId(opCodeName);

    // Sanity check
    if (opCodeId == NOMAD_INVALID_ID) {
        reportInternalError("Unknown op code '" + opCodeName + "'");
    }

    m_unaryOperators.emplace_back(UnaryOperatorRegistration{op, operand, result, opCodeId, fn});
}

void Compiler::registerBinaryOperator(
    BinaryOperator op,
    const Type* lhs,
    const Type* rhs,
    const Type* result,
    const NomadString& opCodeName,
    BinaryFoldingFn fn
) {
    // Sanity check
    if (m_runtime->getInstructionId(opCodeName) == NOMAD_INVALID_ID) {
        reportInternalError("Unknown op code '" + opCodeName + "'");
    }

    // Make sure the operator with the same and type doesn't already exist
    for (auto& definition: m_binaryOperators) {
        if (definition.op == op && definition.lhs == lhs && definition.rhs == rhs) {
            reportInternalError("Binary operator '" + opCodeName + "' already registered for types '" + lhs->getName() + "' and '" + rhs->getName() + "'");
        }
    }

    auto opCodeId = m_runtime->getInstructionId(opCodeName);

    // Sanity check
    if (opCodeId == NOMAD_INVALID_ID) {
        reportInternalError("Unknown op code '" + opCodeName + "'");
    }

    m_binaryOperators.emplace_back(BinaryOperatorRegistration{op, lhs, rhs, result, opCodeId, fn});
}

const Type* Compiler::getUnaryOperatorResultType(UnaryOperator op, const Type* operandType) const {
    for (auto& definition: m_unaryOperators) {
        if (definition.op == op && definition.operand == operandType) {
            return definition.result;
        }
    }

    return nullptr;
}

const Type* Compiler::getBinaryOperatorResultType(BinaryOperator op, const Type* lhsType, const Type* rhsType) const {
    for (auto& definition: m_binaryOperators) {
        if (definition.op == op && definition.lhs == lhsType && definition.rhs == rhsType) {
            return definition.result;
        }
    }

    return nullptr;
}

NomadId Compiler::getUnaryOperatorOpCodeId(UnaryOperator op, const Type* operand) const {
    for (auto& definition: m_unaryOperators) {
        if (definition.op == op && definition.operand == operand) {
            return definition.opCodeId;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Compiler::getBinaryOperatorOpCodeId(BinaryOperator op, const Type* lhs, const Type* rhs) const {
    for (auto& definition: m_binaryOperators) {
        if (definition.op == op && definition.lhs == lhs && definition.rhs == rhs) {
            return definition.opCodeId;
        }
    }

    return NOMAD_INVALID_ID;
}

bool Compiler::foldUnary(
    UnaryOperator op,
    const Type* operandType,
    const ScriptValue& value,
    ScriptValue& result
) const {
    for (auto& fn: m_unaryOperators) {
        if (fn.op == op && fn.operand == operandType) {
            fn.fn(value, result);

            return true;
        }
    }

    return false;
}

bool Compiler::foldBinary(
    BinaryOperator op,
    const Type* lhsType,
    const ScriptValue& lhs,
    const Type* rhsType,
    const ScriptValue& rhs,
    ScriptValue& result
) const {
    for (auto& fn: m_binaryOperators) {
        if (fn.op == op && fn.lhs == lhsType && fn.rhs == rhsType) {
            fn.fn(lhs, rhs, result);

            return true;
        }
    }

    return false;
}

IdentifierType Compiler::getIdentifierType(const NomadString& name, Script* script) {
    if (m_runtime->getKeywordId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Keyword;
    }

    if (isStatement(name)) {
        return IdentifierType::Statement;
    }

    CommandDefinition commandDefinition;

    if (m_runtime->getCommandDefinition(name, commandDefinition)) {
        return IdentifierType::Command;
    }

    if (m_runtime->getConstantId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Constant;
    }

    if (m_runtime->getScriptId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::Script;
    }

    if (m_runtime->getDynamicVariableId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::DynamicVariable;
    }

    if (m_runtime->getVariableContextIdByPrefix(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ContextVariable;
    }

    if (script->getVariableId(name) != NOMAD_INVALID_ID) {
        return IdentifierType::ScriptVariable;
    }

    return IdentifierType::Unknown;
}

void Compiler::getIdentifierDefinition(const NomadString& name, Script* script, IdentifierDefinition& definition) {
    if (name.empty()) {
        definition.identifierType = IdentifierType::Unknown;

        return;
    }

    char firstCharacter = name[0];
    if (!(std::isalnum(firstCharacter) || firstCharacter != '_')) {
        definition.identifierType = IdentifierType::Unknown;

        return;
    }

    definition.identifierType = getIdentifierType(name, script);

    NomadId commandId = m_runtime->getCommandId(name);

    if (commandId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::Command;

        CommandDefinition commandDefinition;

        m_runtime->getCommandDefinition(commandId, commandDefinition);

        definition.commandId = commandId;
        definition.valueType = commandDefinition.returnType;

        return;
    }

    NomadId scriptId = m_runtime->getScriptId(name);

    if (scriptId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::Script;
        definition.variableId = scriptId;
        definition.valueType = m_runtime->getScript(scriptId)->getReturnType();

        return;
    }

    NomadId constantId = m_runtime->getConstantId(name);

    if (constantId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::Constant;
        definition.variableId = constantId;
        definition.valueType = m_runtime->getConstantType(constantId);

        return;
    }

    NomadId dynamicVariableId = m_runtime->getDynamicVariableId(name);

    if (dynamicVariableId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::DynamicVariable;
        definition.variableId = dynamicVariableId;
        definition.valueType = m_runtime->getDynamicVariableType(dynamicVariableId);

        return;
    }

    NomadId contextId = m_runtime->getVariableContextIdByPrefix(name);

    if (contextId != NOMAD_INVALID_ID) {
        auto variableId = m_runtime->getContextVariableId(contextId, name);
        definition.contextId = contextId;
        definition.variableId = variableId;
        definition.valueType = m_runtime->getContextVariableType(contextId, variableId);

        return;
    }

    NomadId variableId = script->getVariableId(name);

    if (variableId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::ScriptVariable;
        definition.variableId = variableId;
        definition.valueType = script->getVariableType(variableId);

        return;
    }

    NomadId parameterId = script->getParameterId(name);

    if (parameterId != NOMAD_INVALID_ID) {
        definition.identifierType = IdentifierType::Parameter;
        definition.variableId = parameterId;
        definition.valueType = script->getParameterType(parameterId);

        return;
    }

    definition.identifierType = IdentifierType::Unknown;
}


NomadIndex Compiler::getOpCodeSize() const {
    return m_instructions.size();
}

NomadIndex Compiler::addOpCode(NomadId opCode) {
    auto fn = m_runtime->getInstructionFn(opCode);

    if (fn == nullptr) {
        reportInternalError("Unknown op code: " + toString(opCode));
    }

    m_instructions.emplace_back(fn);

    return m_instructions.size() - 1;
}

NomadIndex Compiler::addOpCode(const NomadString& opCodeName) {
    auto opCodeId = m_runtime->getInstructionId(opCodeName);

    if (opCodeId == NOMAD_INVALID_ID) {
        reportError("Unknown instruction: " + opCodeName);
    }

    return addOpCode(opCodeId);
}

NomadIndex Compiler::addId(NomadId id) {
    auto index = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(id));

    return index;
}

NomadIndex Compiler::addIndex(NomadIndex index) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(index));

    return instruction;
}

NomadIndex Compiler::addInteger(NomadInteger value) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(value));

    return instruction;
}

NomadIndex Compiler::addFloat(NomadFloat value) {
    auto instruction = m_instructions.size();

    m_instructions.emplace_back(ScriptValue(value));

    return instruction;
}

NomadIndex Compiler::addLoadValue(const Type* type, const ScriptValue& value) {
    if (type == getRuntime()->getBooleanType()) {
        return addLoadBooleanValue(value.getBooleanValue());
    } else if (type == getRuntime()->getIntegerType()) {
        return addLoadIntegerValue(value.getIntegerValue());
    } else if (type == getRuntime()->getFloatType()) {
        return addLoadFloatValue(value.getFloatValue());
    } else if (type == getRuntime()->getStringType()) {
        return addLoadStringValue(value.getStringValue());
    }

    reportInternalError("Unknown type: " + type->getName());

    return NOMAD_INVALID_INDEX;
}

NomadIndex Compiler::addLoadBooleanValue(bool value) {
    if (value) {
        return addOpCode(OpCodes::op_boolean_load_true_r);
    } else {
        return addOpCode(OpCodes::op_boolean_load_false_r);
    }
}

NomadIndex Compiler::addLoadIntegerValue(NomadInteger value) {
    if (value == 0) {
        return addOpCode(OpCodes::op_integer_load_zero_r);
    } else if (value == 1) {
        return addOpCode(OpCodes::op_integer_load_one_r);
    } else {
        addOpCode(OpCodes::op_integer_load_r);

        return addInteger(value);
    }
}

NomadIndex Compiler::addLoadFloatValue(NomadFloat value) {
    if (value == 0) {
        return addOpCode(OpCodes::op_float_load_zero_r);
    } else if (value == 1) {
        return addOpCode(OpCodes::op_float_load_one_r);
    } else {
        addOpCode(OpCodes::op_float_load_r);
        return addFloat(value);
    }
}

NomadIndex Compiler::addLoadStringValue(const NomadString& value) {
    auto stringId = getRuntime()->registerString(value);

    addOpCode(OpCodes::op_string_load_r);

    return addId(stringId);
}

NomadIndex Compiler::addPushResult(const Type* type) {
    if (type == getRuntime()->getStringType()) {
        return addOpCode(OpCodes::op_string_push_r);
    }

    return addOpCode(OpCodes::op_push_r);
}

NomadIndex Compiler::addPopIntermediate(const Type* type) {
    if (type == getRuntime()->getStringType()) {
        return addOpCode(OpCodes::op_string_pop_i);
    }

    return addOpCode(OpCodes::op_pop_i);
}

NomadIndex Compiler::addScriptCall(NomadId targetScriptId) {
    auto script = getRuntime()->getScript(targetScriptId);

    if (script == nullptr) {
        reportInternalError("Unknown script id: " + toString(targetScriptId));
    }
    
    addOpCode(OpCodes::op_call_script);

    if (script->getScriptStart() == NOMAD_INVALID_INDEX) {
        addScriptLink(targetScriptId, m_instructions.size());

        addIndex(NOMAD_INVALID_ID); // Placeholder for script jump index
    } else {
        addIndex(script->getScriptStart());
    }

    NomadIndex parameterCount = script->getParameterCount();

    if (parameterCount != 0) {
        for (auto i = 0; i < parameterCount; i++) {
            auto parameterType = script->getParameterType(i);

            if (parameterType == getRuntime()->getStringType()) {
                addOpCode(OpCodes::op_string_free_stack);
                addIndex(i);
            }
        }

        addOpCode(OpCodes::op_pop_n);
        addIndex(parameterCount);
    }

    return m_instructions.size() - 1;
}

NomadIndex Compiler::addCommandCall(NomadId commandId) {
    CommandDefinition command;

    auto result = m_runtime->getCommandDefinition(commandId, command);

    if (result == false) {
        reportInternalError("Unknown command: " + toString(commandId));
    }

    addOpCode(OpCodes::op_call_command);
    addIndex(commandId);

    if (!command.parameters.empty()) {
        for (auto i = 0; i < command.parameters.size(); i++) {
            auto& parameter = command.parameters[i];

            if (parameter.type == getRuntime()->getStringType()) {
                addOpCode(OpCodes::op_string_free_stack);
                addIndex(i);
            }
        }

        addOpCode(OpCodes::op_pop_n);
        addIndex(command.parameters.size());
    }

    return m_instructions.size() - 1;
}

void Compiler::setOpCode(NomadIndex index, const NomadString& opCodeName) {
    NomadId opCodeId = m_runtime->getInstructionId(opCodeName);

    if (opCodeId == NOMAD_INVALID_ID) {
        reportInternalError("Unknown op code: " + opCodeName);
    }

    setOpCode(index, opCodeId);
}

void Compiler::setOpCode(NomadIndex index, NomadId opCodeId) {
    auto fn = m_runtime->getInstructionFn(opCodeId);

    if (fn == nullptr) {
        reportInternalError("Unknown op code: " + toString(opCodeId));
    }

    m_instructions[index].fn = fn;
}


void Compiler::setId(NomadIndex index, NomadId id) {
    m_instructions[index].value.setIdValue(id);
}

void Compiler::setIndex(NomadIndex index, NomadIndex value) {
    m_instructions[index].value.setIndexValue(value);
}

void Compiler::preParseScript(ScriptFile& scriptFile) {
    auto& tokens = scriptFile.tokens;

    tokens.reset();

    while (tokens.nextLine()) {
        auto& statement = tokens.nextToken();

        PreParseStatementFn preParseStatementFn;

        auto script = m_runtime->getScript(scriptFile.scriptId);

        if (getGetPreParseStatementFn(statement.textValue, preParseStatementFn) && preParseStatementFn) {
            preParseStatementFn(this, script, &tokens);
        }
    }
}


void Compiler::parseScript(ScriptFile& file) {
    auto ast = std::make_unique<ScriptNode>(0, 0);
    auto& tokens = file.tokens;

    tokens.reset();

    while (tokens.nextLine()) {
        auto script = m_runtime->getScript(file.scriptId);

        auto statement = parser::parseLine(this, script, &tokens);

        if (statement) {
            ast->addStatement(std::move(statement));
        }
    }

    setScriptNode(file.scriptId, std::move(ast));
}

void Compiler::compileScript(ScriptSource& source) {
    Script* script = source.script;

    script->setScriptStart(m_instructions.size());

    // Initialize string variables.
    NomadIndex variableCount = script->getVariableCount();

    if (variableCount != 0) {
        addOpCode(OpCodes::op_push_n);
        addIndex(variableCount);
        for (auto i = 0; i < variableCount; i++) {
            if (script->getVariableType(i) == getRuntime()->getStringType()) {
                addOpCode(OpCodes::op_string_init);
                addIndex(i);
            }
        }
    }

    source.ast->compile(this, script);

    // Free string variables.
    if (variableCount != 0) {
        for (auto i = 0; i < variableCount; i++) {
            if (script->getVariableType(i) == getRuntime()->getStringType()) {
                addOpCode(OpCodes::op_string_free_variable);
                addIndex(i);
            }

        }
        addOpCode(OpCodes::op_return_n);
        addIndex(variableCount);
    } else {
        addOpCode(OpCodes::op_return);
    }


    script->setScriptEnd(m_instructions.size());
}

NomadId Compiler::registerScriptFile(
    const NomadString& scriptName,
    const NomadString& fileName,
    const NomadString& source
) {
    auto scriptFile = ScriptFile {
        scriptName,
        fileName,
        source,
        {m_runtime, source},
    };

    // Also register script so it is available for linking
    scriptFile.scriptId = registerScriptSource(scriptName, fileName, source);

    m_files.emplace_back(std::move(scriptFile));

    // Sort scripts alphabetically to ensure consistent order in compiling across systems.
    std::sort(m_files.begin(), m_files.end(), [](const ScriptFile& a, const ScriptFile& b) {
        return a.scriptName < b.scriptName;
    });

    return scriptFile.scriptId;
}

NomadId Compiler::registerScriptSource(
    const NomadString& scriptName,
    const NomadString& fileName,
    const NomadString& source
) {
    auto scriptId = m_runtime->registerScript(scriptName, fileName, source);

    if (scriptId == NOMAD_INVALID_ID) {
        reportError("Could not register script '" + scriptName + "' (" + fileName + ")");
    }

    auto script = m_runtime->getScript(scriptId);

    m_sources.emplace_back(ScriptSource {
        scriptName,
        {},
        script
    });

    return scriptId;
}

void Compiler::setScriptNode(NomadId scriptId, std::unique_ptr<ScriptNode> ast) {
    for (auto& source: m_sources) {
        if (source.script->getId() == scriptId) {
            source.ast = std::move(ast);

            return;
        }
    }

    reportInternalError("Unknown script id: " + toString(scriptId));
}

void Compiler::loadScriptsFromPath(const NomadString& path) {
    scanDirectoryForScripts(path, "", 10);
}

void Compiler::compileScripts() {
    log::info("Pre-Parsing scripts");

    for (auto& scriptFile: m_files) {
        try {
            log::debug("Pre-Parsing '" + scriptFile.scriptName + "' (" + scriptFile.fileName + ")");

            preParseScript(scriptFile);
        } catch (std::exception& e) {
            reportError("Failed to pre-parse script '" + scriptFile.scriptName + "' (" + scriptFile.fileName + "): " + e.what());
        }
    }

    log::info("Parsing scripts");

    for (auto& scriptFile: m_files) {
        try {
            log::debug("Parsing '" + scriptFile.scriptName + "' (" + scriptFile.fileName + ")");

            parseScript(scriptFile);
        } catch (std::exception& e) {
            reportError("Failed to parse script '" + scriptFile.scriptName + "' (" + scriptFile.fileName + "): " + e.what());
        }
    }

    log::info("Syntax checking scripts");

    for (auto& source: m_sources) {
        try {
            log::debug("Syntax checking script '" + source.script->getName() + "' (" + source.script->getPath() + ")");

            source.ast->parse(this, source.script);
        } catch (std::exception& e) {
            reportError("Failed to syntax check script '" + source.script->getName() + "' (" + source.script->getPath() + "): " + e.what());
        }
    }

    log::info("Compiling scripts");

    for (auto& source: m_sources) {
        try {
            log::debug("Compiling script '" + source.script->getName() + "' (" + source.script->getPath() + ")");

            compileScript(source);
        } catch (std::exception& e) {
            reportError("Failed to compile script '" + source.script->getName() + "' (" + source.script->getPath() + "): " + e.what());
        }
    }

    log::info("Linking scripts");

    linkScripts();

    m_scriptLinks.clear();

    log::info("Scripts compiled");
}

void Compiler::addScriptLink(NomadId scriptId, NomadIndex callIndex) {
    m_scriptLinks.emplace_back(ScriptLink{scriptId, callIndex});
}

void Compiler::linkScripts() {
    for (auto& link: m_scriptLinks) {
        auto script = m_runtime->getScript(link.scriptId);

        if (script == nullptr) {
            reportError("Failed to link script '" + toString(link.scriptId) + "'");
        }
        if (script->getScriptStart() == NOMAD_INVALID_INDEX) {
            reportError("Script '" + script->getName() + "' has not been compiled");
        }

        setIndex(link.callIndex, script->getScriptStart());
    }
}

void Compiler::scanDirectoryForScripts(const NomadString& basePath, const NomadString& sub_path, NomadIndex max_depth) {
    if (max_depth == 0) {
        log::debug("Skipping directory '" + sub_path + "' (max depth reached)");

        return;
    }

    const NomadString extension = ".nomad";

    auto path_string = concatPath(basePath, sub_path);
    std::filesystem::path path{path_string};

    for (auto& entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            auto directory_name = NomadString(entry.path().filename().string());
            auto new_sub_path = concatPath(sub_path, directory_name);
            scanDirectoryForScripts(basePath, new_sub_path, max_depth - 1);
        } else if (entry.is_regular_file() && entry.path().extension() == extension) {
            auto file_name = basePath + "/" + sub_path + "/" + NomadString(entry.path().filename().string());

            auto start = basePath.length() + 1;
            auto length = entry.path().string().length() - start - extension.length();

            auto script_name = NomadString(entry.path().string().substr(start, length));
            std::replace(script_name.begin(), script_name.end(), '\\', '.');
            std::replace(script_name.begin(), script_name.end(), '/', '.');

            // Read source
            std::ifstream file(file_name.c_str(), std::ios::in);
            if (!file.is_open()) {
                reportError("Failed to open file '" + file_name + "'");
            }

            NomadString source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

            registerScriptFile(script_name, file_name, source);
       }
    }
}

NomadString Compiler::generateScriptName(const NomadString& generatedName, const NomadString& hostScriptName, NomadIndex line) {
    NomadString generatedScriptName;
    auto index = 0;

    do {
        generatedScriptName = SCRIPT_INTERNAL_NAME_PREFIX + generatedName + "_" + hostScriptName + "_" + std::to_string(line);
    } while (m_runtime->getScriptId(generatedScriptName) != NOMAD_INVALID_ID);

    return generatedScriptName;
}

NomadString Compiler::generateScriptName(const NomadString& name, const Script* script, NomadIndex line) {
    return generateScriptName(name, script->getName(), line);
}

} // nomad