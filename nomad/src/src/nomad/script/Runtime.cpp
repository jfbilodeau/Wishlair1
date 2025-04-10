//
// Created by jfbil on 2023-06-04.
//

#include "nomad/script/Runtime.hpp"

#include "nomad/compiler/Compiler.hpp"

#include "nomad/script/Commands.hpp"
#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Type.hpp"

#include <utility>

#include "boost/json/array.hpp"

#include <iomanip>

namespace nomad {

Runtime::Runtime() {
    // Types
    registerType(std::make_unique<VoidType>());
    registerType(std::make_unique<IdType>());
    registerType(std::make_unique<FloatType>());
    registerType(std::make_unique<IntegerType>());
    registerType(std::make_unique<BooleanType>());
    registerType(std::make_unique<StringType>());

    // Register instructions
    registerDefaultInstructions();

    // Register build-in commands
    registerBuildInCommands(this);

//    register_keyword("stringId" NomadDoc("Load the string id of the given string"));
    registerKeyword("fun", NomadDoc("Declares a function"));
    registerKeyword("else", NomadDoc("`else` branch of an `if` statement"));
    registerKeyword("endIf", NomadDoc("End of an `if` statement"));
    registerKeyword("end", NomadDoc("End of a `fun` statement"));

    registerConstant("false", ScriptValue(NOMAD_FALSE), getBooleanType());
    registerConstant("true", ScriptValue(NOMAD_TRUE), getBooleanType());
    registerConstant("pi", ScriptValue(NOMAD_PI), getFloatType());

    registerVariableContext("global", "global.", std::make_unique<SimpleVariableContext>());
}

Runtime::~Runtime() {
}

void Runtime::setDebug(bool debug) {
    m_debug = debug;
}

bool Runtime::isDebug() const {
    return m_debug;
}

const std::vector<Instruction>& Runtime::getInstructions() const {
    return m_instructions;
}

NomadId Runtime::registerCommand(
    const NomadString& name,
    CommandFn command_fn,
    const std::vector<CommandParameterDefinition>& parameters,
    const Type* returnType,
    NomadDocArg
) {
    NomadId id = getCommandId(name);

    if (id != NOMAD_INVALID_ID) {
        log::error("Command '" + name + "' already registered");

        return NOMAD_INVALID_ID;
    }

    id = to_nomad_id(m_commands.size());

    m_commands.push_back(
        {
            id,
            name,
            std::move(command_fn),
            parameters,
            returnType,
            doc
        }
    );

    return id;
}

NomadId Runtime::registerType(std::unique_ptr<Type>&& type) {
    m_types.push_back(std::move(type));

    return to_nomad_id(m_types.size() - 1);
}

const Type* Runtime::getType(NomadId id) const {
    return m_types[id].get();
}

const Type* Runtime::getType(const NomadString& name) const {
    for (const auto& type: m_types) {
        if (type->getName() == name) {
            return type.get();
        }
    }

    return nullptr;
}

const Type* Runtime::getCallbackType(const std::vector<const Type*>& parameterTypes, const Type* returnType) {
    auto test_type = std::make_unique<ScriptType>(parameterTypes, returnType);

    for (const auto& type: m_types) {
        if (type->same_type(test_type.get())) {
            return type.get();
        }
    }

    m_types.push_back(std::move(test_type));

    return m_types.back().get();
}

const Type* Runtime::getPredicateType() {
    return getCallbackType({}, getBooleanType());
}

NomadId Runtime::registerInstruction(
    const NomadString& name,
    InstructionFn fn,
    const NomadString& doc,
    std::vector<Operand> operands
) {
    auto id = getInstructionId(name);

    if (id != NOMAD_INVALID_ID) {
        log::warning("Instruction '" + name + "' already registered");

        return id;
    }

    id = to_nomad_id(m_opCodes.size());

    m_opCodes.emplace_back(
        OpCodeDefinition{
            id,
            name,
            fn,
            std::move(operands),
            doc
        }
    );

    return id;
}

const Type* Runtime::getVoidType() const {
    return getType(VOID_TYPE_NAME);
}
//
//const Type* Runtime::get_id_type() const {
//    return get_type(ID_TYPE_NAME);
//}

const Type* Runtime::getBooleanType() const {
    return getType(BOOLEAN_TYPE_NAME);
}

const Type* Runtime::getIntegerType() const {
    return getType(INTEGER_TYPE_NAME);
}

const Type* Runtime::getFloatType() const {
    return getType(FLOAT_TYPE_NAME);
}

const Type* Runtime::getStringType() const {
    return getType(STRING_TYPE_NAME);
}

const Type* Runtime::getScriptType() const {
    return getType(SCRIPT_TYPE_NAME);
}

NomadId Runtime::getInstructionId(const NomadString& name) const {
    for (auto i = 0; i < m_opCodes.size(); ++i) {
        if (m_opCodes[i].name == name) {
            return static_cast<NomadId>(i);
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Runtime::getInstructionId(InstructionFn fn) const {
    for (auto& op_code: m_opCodes) {
        if (op_code.fn == fn) {
            return op_code.id;
        }
    }

    return NOMAD_INVALID_ID;
}

InstructionFn Runtime::getInstructionFn(NomadId id) const {
    return m_opCodes[id].fn;
}

const NomadString& Runtime::getInstructionName(NomadId id) const {
    return m_opCodes[id].name;
}

const std::vector<Operand>& Runtime::getInstructionOperands(NomadId id) const {
    return m_opCodes[id].operands;
}

NomadId Runtime::getCommandId(const NomadString& name) const {
    for (auto i = 0; i < m_commands.size(); ++i) {
        if (m_commands[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

CommandFn Runtime::getCommandFn(NomadId id) const {
    return m_commands[id].fn;
}

//NomadId Runtime::register_statement(const String& name, ParseStatementFn fn) {
//    auto id = to_nomad_id(m_statements.size());
//
//    ParseStatementFnRegistration registration;
//
//    registration.id = id;
//    registration.name = name;
//    registration.fn = fn;
//
//    m_statements.push_back(registration);
//
//    return id;
//}

//CompilerStatementFn Runtime::get_statement(const String& name) const {
//    for (const auto& statement: m_statements) {
//        if (statement.name == name) {
//            return statement.fn;
//        }
//    }
//
//    return nullptr;
//}
//
//void Runtime::get_statements(std::vector<CompilerStatementFnRegistration>& statements) const {
//    statements = m_statements;
//}
//
bool Runtime::getCommandDefinition(NomadId id, CommandDefinition& definition) const {
    if (id >= m_commands.size()) {
        return false;
    }

    definition.id = id;
    definition.name = m_commands[id].name;
    definition.fn = m_commands[id].fn;
    definition.parameters = m_commands[id].parameters;
    definition.returnType = m_commands[id].returnType;

    return true;
}

bool Runtime::getCommandDefinition(const NomadString& name, CommandDefinition& definition) const {
    auto id = getCommandId(name);

    if (id == NOMAD_INVALID_ID) {
        return false;
    }

    return getCommandDefinition(id, definition);
}

void Runtime::getCommands(std::vector<CommandDefinition>& commands) const {
    for (auto& command: m_commands) {
        CommandDefinition definition;

        definition.id = command.id;
        definition.name = command.name;
        definition.fn = command.fn;
        definition.parameters = command.parameters;
#if defined(NOMAD_SCRIPT_DOC)
        definition.doc = command.doc;
#endif

        commands.push_back(definition);
    }
}

NomadId Runtime::registerKeyword(const NomadString& keyword, NomadDocArg) {
    auto id = to_nomad_id(m_keywords.size());

    KeywordDefinition definition;

    definition.keyword = keyword;
#if defined(NOMAD_SCRIPT_DOC)
    definition.doc = doc;
#endif

    m_keywords.push_back(definition);

    return id;
}

NomadId Runtime::getKeywordId(const NomadString& keyword) const {
    for (auto i = 0; i < m_keywords.size(); ++i) {
        if (m_keywords[i].keyword == keyword) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

void Runtime::getKeywords(std::vector<KeywordDefinition>& keywords) const {
    keywords = m_keywords;
}

NomadId Runtime::registerConstant(const NomadString& name, const ScriptValue& value, const Type* type) {
    if (getConstantId(name) != NOMAD_INVALID_ID) {
        log::error("Constant with name '" + name + "' already registered.");

        return NOMAD_INVALID_ID;
    }

    auto constant_id = m_constantsMap.registerVariable(name, type);

    m_constants.resize(constant_id + 1);
    type->copyValue(value, m_constants[constant_id]);

    return constant_id;
}

NomadId Runtime::getConstantId(const NomadString& name) const {
    return m_constantsMap.getVariableId(name);
}

void Runtime::getConstantValue(NomadId id, ScriptValue& value) const {
    value = m_constants[id];
}

const NomadString& Runtime::getConstantName(NomadId id) const {
    return m_constantsMap.getVariableName(id);
}

const Type* Runtime::getConstantType(NomadId id) const {
    return m_constantsMap.getVariableType(id);
}

NomadId Runtime::registerString(const NomadString& string) {
    auto id = getStringId(string);

    if (id != NOMAD_INVALID_ID) {
        return id;
    }

    id = to_nomad_id(m_strings.size());

    m_strings.push_back(string);

    return id;
}

NomadId Runtime::getStringId(const NomadString& string) const {
    for (auto i = 0; i < m_strings.size(); ++i) {
        if (m_strings[i] == string) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

const NomadString& Runtime::getString(NomadId string_id) const {
    return m_strings[string_id];
}

const NomadString& Runtime::getStringByName(const NomadString& name) const {
    auto string_id = getStringId(name);

    return getString(string_id);
}

NomadId Runtime::registerFormatString(const NomadString& formatString, NomadId scriptId) {
    auto id = getFormatStringId(formatString, scriptId);

    if (id != NOMAD_INVALID_ID) {
        return id;
    }

    id = to_nomad_id(m_formatStrings.size());

    m_formatStrings.push_back(
        {
            id,
            scriptId,
            std::make_unique<FormatString>(formatString, scriptId)
        }
    );

    return id;
}

FormatString* Runtime::getFormatString(NomadId id) const {
    if (id >= m_formatStrings.size()) {
        return nullptr;
    }

    return m_formatStrings[id].formatString.get();
}

NomadId Runtime::getFormatStringId(const NomadString& formatString, NomadId scriptId) const {
    for (auto& i: m_formatStrings) {
        if (i.scriptId == scriptId && i.formatString->getFormatString() == formatString) {
            return i.id;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Runtime::registerVariableContext(
    const NomadString& name,
    const NomadString& prefix,
    std::unique_ptr<VariableContext> context
) {
    NomadId id = to_nomad_id(m_variables.size());

    auto registration = VariableContextRegistration{
        id,
        name,
        prefix,
        std::move(context)
    };

    m_variables.emplace_back(std::move(registration));

    return id;
}

NomadId Runtime::getContextId(const NomadString& name) const {
    for (auto i = 0; i < m_variables.size(); ++i) {
        if (m_variables[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

VariableContext* Runtime::getVariableContext(NomadId contextId) const {
    if (contextId >= m_variables.size()) {
        return nullptr;
    }

    return m_variables[contextId].context.get();
}

NomadString Runtime::getContextName(NomadId id) const {
    return m_variables[id].name;
}

NomadId Runtime::getContextVariableId(NomadId context_id, const NomadString& variableName) {
    auto variable_id = m_variables[context_id].context->getVariableId(variableName);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = m_variables[context_id].context->registerVariable(variableName, nullptr);
    }

    return variable_id;
}

NomadId Runtime::getVariableContextIdByPrefix(const NomadString& variableName) const {
    for (const auto& context: m_variables) {
        if (variableName.rfind(context.prefix, 0) == 0) {
            return context.id;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadString Runtime::getContextVariableName(NomadId contextId, NomadId variableId) const {
    return m_variables[contextId].context->getVariableName(variableId);
}

const Type* Runtime::getContextVariableType(NomadId contextId, NomadId variableId) const {
    return m_variables[contextId].context->getVariableType(variableId);
}

void Runtime::setContextVariableValue(NomadId contextId, NomadId variableId, const ScriptValue& value) const {
    m_variables[contextId].context->setValue(variableId, value);
}

void Runtime::getContextVariableValue(NomadId contextId, NomadId variableId, ScriptValue& value) const {
    m_variables[contextId].context->getValue(variableId, value);
}

void
Runtime::setStringContextVariableValue(NomadId contextId, NomadId variableId, const NomadString& value) const {
    ScriptValue string_value(value);
    m_variables[contextId].context->setValue(variableId, string_value);
}

void Runtime::setStringContextVariableValue(NomadId contextId, NomadId variableId, const NomadChar* value) const {
    ScriptValue string_value(value);
    m_variables[contextId].context->setValue(variableId, string_value);
}

void Runtime::get_string_context_variable_value(NomadId contextId, NomadId variableId, NomadString& value) const {
    ScriptValue string_value;
    m_variables[contextId].context->getValue(variableId, string_value);

    value = string_value.getStringValue();
}

NomadId Runtime::registerScript(
    const NomadString& name,
    const NomadString& path,
    const NomadString& source
) {
    if (getScriptId(name) != NOMAD_INVALID_ID) {
        log::error("Script with name '" + name + "' already registered.");

        return NOMAD_INVALID_ID;
    }

    auto id = to_nomad_id(m_scripts.size());

    auto script = new Script(id, name, path, source);

    m_scripts.push_back(script);

    return id;
}

NomadId Runtime::getScriptId(const NomadString& name) const {
    for (auto i = 0; i < m_scripts.size(); ++i) {
        if (m_scripts[i]->getName() == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

Script* Runtime::getScript(NomadId script_id) const {
    return m_scripts[script_id];
}

NomadIndex Runtime::getScriptCount() const {
    return m_scripts.size();
}

void Runtime::getScripts(std::vector<Script*>& scripts) const {
    scripts = m_scripts;
}

void Runtime::getScripts(TempVector<Script*>& scripts) {
    scripts.clear();

    scripts.insert(scripts.end(), m_scripts.begin(), m_scripts.end());
}

NomadIndex Runtime::getScriptSize() const {
    NomadIndex size = 0;

    for (const auto& script: m_scripts) {
        size += script->getScriptLength();
    }

    return size;
}

std::unique_ptr<Compiler> Runtime::createCompiler() {
    return std::make_unique<Compiler>(this, m_instructions);
}

NomadId Runtime::registerDynamicVariable(
    const NomadString& name,
    DynamicVariableSetFn setFn,
    DynamicVariableGetFn getFn,
    const Type* type,
    NomadDocArg
) {
    auto id = to_nomad_id(m_dynamicVariables.size());

    DynamicVariableRegistration definition;
    definition.name = name;
    definition.type = type;
    definition.setFn = std::move(setFn);
    definition.getFn = std::move(getFn);
#if defined(NOMAD_SCRIPT_DOC)
    definition.doc = doc;
#endif

    m_dynamicVariables.push_back(definition);

    return id;
}

NomadId Runtime::getDynamicVariableId(const NomadString& name) const {
    for (auto i = 0; i < m_dynamicVariables.size(); ++i) {
        if (m_dynamicVariables[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

NomadString Runtime::getDynamicVariableName(NomadId id) const {
    return m_dynamicVariables[id].name;
}

const Type* Runtime::getDynamicVariableType(NomadId id) const {
    return m_dynamicVariables[id].type;
}

bool Runtime::canSetDynamicVariable(NomadId id) const {
    return m_dynamicVariables[id].setFn != nullptr;
}

bool Runtime::canGetDynamicVariable(NomadId id) const {
    return m_dynamicVariables[id].getFn != nullptr;
}

void Runtime::setDynamicVariable(Interpreter* interpreter, NomadId id, const ScriptValue& value) {
    m_dynamicVariables[id].setFn(interpreter, value);
}

void Runtime::getDynamicVariableValue(Interpreter* interpreter, NomadId id, ScriptValue& value) {
    m_dynamicVariables[id].getFn(interpreter, value);
}

void Runtime::setStringDynamicVariable(Interpreter* interpreter, NomadId id, const NomadString& value) {
    setStringDynamicVariable(interpreter, id, value.c_str());
}

void Runtime::setStringDynamicVariable(Interpreter* interpreter, NomadId id, const NomadChar* value) {
    ScriptValue string_value(value);
    setStringDynamicVariable(interpreter, id, string_value);
}

void Runtime::setStringDynamicVariable(Interpreter* interpreter, NomadId id, const ScriptValue& value) {
    m_dynamicVariables[id].setFn(interpreter, value);
}

void Runtime::getStringDynamicVariableValue(Interpreter* interpreter, NomadId id, NomadString& value) {
    ScriptValue string_value;

    getStringDynamicVariableValue(interpreter, id, string_value);

    value = string_value.getStringValue();
}

void Runtime::getStringDynamicVariableValue(Interpreter* interpreter, NomadId id, ScriptValue& value) {
    m_dynamicVariables[id].getFn(interpreter, value);
}

//std::shared_ptr<Logger> Runtime::get_logger() const {
//    return m_logger;
//}
//
//void Runtime::get_opcode_definitions(std::vector<OpCodeDefinition>& definitions) const {
//    definitions = m_op_codes;
//}

std::unique_ptr<Interpreter> Runtime::createInterpreter(NomadId scriptId) {
    auto script = m_scripts[scriptId];

    return std::make_unique<Interpreter>(this);
}

void Runtime::executeScript(NomadId scriptId) {
    if (m_interpreter == nullptr) {
        m_interpreter = std::make_unique<Interpreter>(this);
    }

    auto script = m_scripts[scriptId];

     return m_interpreter->run(script, {});
}

void Runtime::executeScript(NomadId scriptId, ScriptValue& returnValue) {
    auto script = m_scripts[scriptId];

    Interpreter interpreter(this);

    interpreter.run(script, {});

    returnValue = interpreter.getResult();
}

void Runtime::dumpInstructions(std::ostream& out) {
    NomadId script_id = 0;
    auto script = getScript(script_id);

    for (auto i = 0; i < m_instructions.size(); ++i) {
        // Should we move to the next script?
        if (i == script->getScriptEnd()) {
            script_id++;
            script = getScript(script_id);
        }

        auto fn = m_instructions[i].fn;

        auto instruction_id = getInstructionId(fn);

        if (instruction_id == NOMAD_INVALID_ID) {
            out << std::setfill('0') << std::setw(4) << i << ": Unknown instruction" << std::endl;
            continue;
        }

        auto& name = getInstructionName(instruction_id);
        auto& operands = getInstructionOperands(instruction_id);

        out << std::setfill('0') << std::setw(4) << i << ": " << name << std::endl;

        for (auto operand : operands) {
            i++;

            auto& value = m_instructions[i].value;

            NomadString text_value;

            switch (operand) {
                case Operand::Boolean:
                    text_value = value.getBooleanValue() ? "true" : "false";
                    break;
                case Operand::Integer:
                    text_value = std::to_string(value.getIntegerValue());
                    break;
                case Operand::Float:
                    text_value = std::to_string(value.getFloatValue());
                    break;
                case Operand::String:
                    text_value = "\"" + getString(value.getIdValue()) + "\"";
                    break;
                case Operand::Id:
                    text_value = std::to_string(value.getIdValue());
                    break;
                case Operand::Script:
                    text_value = getScript(value.getIdValue())->getName();
                    break;
                case Operand::Command: {
                    CommandDefinition definition;
                    auto result = getCommandDefinition(value.getIdValue(), definition);

                    if (result) {
                        text_value = definition.name;
                    } else {
                        text_value = "<unknown command>";
                    }
                    break;
                }
                case Operand::ScriptVariable:
                    text_value = script->getVariableName(value.getIdValue());
                    break;
                case Operand::DynamicVariable:
                    text_value = getDynamicVariableName(value.getIdValue());
                    break;
                case Operand::ContextId: {
                    auto context_id = value.getIdValue();
                    auto variable_id = m_instructions[i+1].value.getIdValue();
                    text_value = getContextVariableName(context_id, variable_id);
                    break;
                }
                case Operand::ContextVariableId:
                    continue; // Skip over context variable id
                    log::debug("ContextVariableId operand type should have already been handled");
                    break;
                case Operand::FormatString:
                    text_value = "$\"" + getFormatString(value.getIdValue())->getFormatString() + "\"";
                    break;
                default:
                    text_value = "unknown";
                    log::debug("Unknown operand type: " + std::to_string(static_cast<int>(operand)));
            }

            out
                << std::setfill('0')
                << std::setw(4)
                << i
                << ": "
                << text_value
                << std::endl;
        }

        out.flush();
    }
}

void Runtime::dumpDocumentation(std::ostream &out) {
    // Output TOC
    out << "# Nomad Engine Reference" << std::endl;

    out << "* [Constants](#Constants)" << std::endl;
    out << "* [Variable contexts](#Variable-contexts)" << std::endl;
    out << "* [Commands](#Commands)" << std::endl;
    out << "* [Instructions](#Instructions)" << std::endl;

    out << "---" << std::endl;

    out << "## Constants" << std::endl;

    for (auto i = 0; i < m_constantsMap.getVariableCount(); ++i) {
        ScriptValue constant_value;
        NomadString constant_text_value;

        auto constant_type = getConstantType(i);
        getConstantValue(i, constant_value);

        if (constant_type == getVoidType()) {
            constant_text_value = "";
        } else if (constant_type == getBooleanType()) {
            constant_text_value = constant_value.getBooleanValue() ? "true" : "false";
        } else if (constant_type == getIntegerType()) {
            constant_text_value = std::format("{0} ({0:#0x})", constant_value.getIntegerValue());
        } else if (constant_type == getFloatType()) {
            constant_text_value = std::format("{:}", constant_value.getFloatValue());
        } else if (constant_type == getStringType()) {
            constant_text_value = std::format("\"{}\"", constant_value.getStringValue());
        } else if (constant_type == getScriptType()) {
            constant_text_value = getScript(constant_value.getIdValue())->getName();
        } else {
            constant_text_value = "<unknown_type>";
        }

        out
            << "* `"
            << getConstantName(i)
            << ":"
            << getConstantType(i)->getName()
            << " = "
            << constant_text_value
            << "`"
            << std::endl;
    }

    out << std::endl;

    out << "## Variable contexts" << std::endl;

    for (auto& context: m_variables) {
        out << "- `" << context.prefix << "`" << " (" << context.name << ")" << std::endl;
    }

    out << std::endl;

    out << "## Commands" << std::endl;

    for (auto& command: m_commands) {
        out << "`" << command.name << "`" << std::endl;

        for (auto& parameter: command.parameters) {
            out << "- `" << parameter.name << ":" << parameter.type->getName() << "`" << std::endl;
        }

        out << std::endl;

        out << "`return " << command.returnType->getName() << "`" << std::endl;

        out << std::endl;

        out << command.doc << std::endl;
        out << std::endl;
        out << "---" << std::endl;
    }

    out << std::endl;

    out << "## Instructions" << std::endl;

    for (auto& op_code: m_opCodes) {
        out << "`" << op_code.name << "`" << std::endl;
        out << op_code.doc << std::endl;
        out << std::endl;
    }

    out.flush();
}

} // namespace nomad