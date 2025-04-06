//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Documentation.hpp"
#include "nomad/script/DynamicVariable.hpp"
#include "nomad/script/Script.hpp"
#include "nomad/script/VariableContext.hpp"

namespace nomad {

// Forward declarations
class Compiler;
class FormatString;
class Interpreter;
class Parser;
class ScriptBuilder;
class Tokenizer;
class Type;

enum class Operand {
    Boolean,
    Integer,
    Float,
    String,
    Id,
    Script,
    Command,
    ScriptVariable,
    DynamicVariable,
    ContextId,
    ContextVariableId,
    FormatString,
};

//using CompilerStatementFn = void (*)(Compiler*, Tokenizer*, ScriptBuilder*);
struct OpCodeDefinition {
    NomadId id;
    NomadString name;
    InstructionFn fn;
    std::vector<Operand> operands;
    NomadDocField;
};

struct KeywordDefinition {
    NomadString keyword;
    NomadDocField;
};

class Runtime {
private:
    struct DynamicVariableRegistration {
        NomadString name;
        const Type* type;
        DynamicVariableSetFn setFn;
        DynamicVariableGetFn getFn;
        NomadDocField;
    };

    struct ContextVariable {
        NomadString name;
        Type* type;
        ScriptValue value;
    };

    struct VariableContextRegistration {
        NomadId id;
        NomadString name;
        NomadString prefix;
        std::unique_ptr<VariableContext> context;
    };

public:
    Runtime();
    Runtime(const Runtime&) = delete;
    ~Runtime();

    // Generate opcodes that are easier to debug
    // ie: replace op_load_constant with op_load_value
    void setDebug(bool debug);
    [[nodiscard]] bool isDebug() const;

    // Instructions
    [[nodiscard]] const std::vector<Instruction>& getInstructions() const;

    // Types
    NomadId registerType(std::unique_ptr<Type>&& type);
    [[nodiscard]] const Type* getType(NomadId id) const;
    [[nodiscard]] const Type* getType(const NomadString& name) const;
    [[nodiscard]] const Type* getCallbackType(const std::vector<const Type*>& parameterTypes, const Type* returnType);
    [[nodiscard]] const Type* getPredicateType();

    // Convenience type access:
    [[nodiscard]] const Type* getVoidType() const;
    [[nodiscard]] const Type* getBooleanType() const;
    [[nodiscard]] const Type* getIntegerType() const;
    [[nodiscard]] const Type* getFloatType() const;
    [[nodiscard]] const Type* getStringType() const;
    [[nodiscard]] const Type* getScriptType() const;

    // OpCodes
    NomadId registerInstruction(const NomadString& name, InstructionFn fn, NomadDocArg, std::vector<Operand> operands);
    [[nodiscard]] NomadId getInstructionId(const NomadString& name) const;
    [[nodiscard]] NomadId getInstructionId(InstructionFn fn) const;
    [[nodiscard]] InstructionFn getInstructionFn(NomadId id) const;
    [[nodiscard]] const NomadString& getInstructionName(NomadId id) const;
    [[nodiscard]] const std::vector<Operand>& getInstructionOperands(NomadId id) const;

    // Commands
    NomadId registerCommand(const NomadString& name, CommandFn command_fn, const std::vector<CommandParameterDefinition>& parameters, const Type* returnType, NomadDocArg);
    [[nodiscard]] NomadId getCommandId(const NomadString& name) const;
    [[nodiscard]] CommandFn getCommandFn(NomadId id) const;

    bool getCommandDefinition(NomadId id, CommandDefinition& definition) const;
    bool getCommandDefinition(const NomadString& name, CommandDefinition& definition) const;
    void getCommands(std::vector<CommandDefinition>& commands) const;

    // Keywords
    NomadId registerKeyword(const NomadString& keyword, NomadDocArg);
    [[nodiscard]] NomadId getKeywordId(const NomadString& keyword) const;
    void getKeywords(std::vector<KeywordDefinition>& keywords) const;

    // Constants
    NomadId registerConstant(const NomadString& name, const ScriptValue& value, const Type* type);
    void getConstantValue(NomadId id, ScriptValue& value) const;
    [[nodiscard]] NomadId getConstantId(const NomadString& name) const;
    [[nodiscard]] const NomadString& getConstantName(NomadId id) const;
    [[nodiscard]] const Type* getConstantType(NomadId id) const;

    // Static strings
    NomadId registerString(const NomadString& string);
    [[nodiscard]] NomadId getStringId(const NomadString& string) const;
    [[nodiscard]] const NomadString& getString(NomadId id) const;
    [[nodiscard]] const NomadString& getStringByName(const NomadString& name) const;

    // Format strings
    NomadId registerFormatString(const NomadString& formatString, NomadId scriptId);
    [[nodiscard]] FormatString* getFormatString(NomadId id) const;
    [[nodiscard]] NomadId getFormatStringId(const NomadString& formatString, NomadId scriptId) const;

    // Dynamic variables
    NomadId registerDynamicVariable(const NomadString& name, DynamicVariableSetFn setFn, DynamicVariableGetFn getFn, const Type* type, NomadDocArg);
    [[nodiscard]] NomadId getDynamicVariableId(const NomadString& name) const;
    [[nodiscard]] NomadString getDynamicVariableName(NomadId id) const;
    [[nodiscard]] const Type* getDynamicVariableType(NomadId id) const;
    [[nodiscard]] bool canSetDynamicVariable(NomadId id) const;
    [[nodiscard]] bool canGetDynamicVariable(NomadId id) const;
    void setDynamicVariable(Interpreter* interpreter, NomadId id, const ScriptValue& value);
    void getDynamicVariableValue(Interpreter* interpreter, NomadId id, ScriptValue& value);
    void setStringDynamicVariable(Interpreter* interpreter, NomadId id, const NomadString& value);
    void setStringDynamicVariable(Interpreter* interpreter, NomadId id, const NomadChar* value);
    void setStringDynamicVariable(Interpreter* interpreter, NomadId id, const ScriptValue& value);

    void getStringDynamicVariableValue(Interpreter* interpreter, NomadId id, NomadString& value);
    void getStringDynamicVariableValue(Interpreter* interpreter, NomadId id, ScriptValue& value);

    // Variable contexts
    NomadId registerVariableContext(const NomadString& name, const NomadString& prefix, std::unique_ptr<VariableContext> context);
    [[nodiscard]] NomadId getContextId(const NomadString& name) const;
    [[nodiscard]] VariableContext* getVariableContext(NomadId contextId) const;
    [[nodiscard]] NomadString getContextName(NomadId id) const;
    NomadId getContextVariableId(NomadId context_id, const NomadString& variableName);
    [[nodiscard]] NomadId getVariableContextIdByPrefix(const NomadString& variableName) const;
    [[nodiscard]] NomadString getContextVariableName(NomadId contextId, NomadId variableId) const;
    [[nodiscard]] const Type* getContextVariableType(NomadId contextId, NomadId variableId) const;
    void setContextVariableValue(NomadId contextId, NomadId variableId, const ScriptValue& value) const;
    void getContextVariableValue(NomadId contextId, NomadId variableId, ScriptValue& value) const;
    void setStringContextVariableValue(NomadId contextId, NomadId variableId, const NomadString& value) const;
    void setStringContextVariableValue(NomadId contextId, NomadId variableId, const NomadChar* value) const;
    void get_string_context_variable_value(NomadId contextId, NomadId variableId, NomadString& value) const;

    // Scripts
    NomadId registerScript(const NomadString& name, const NomadString& path, const NomadString& source);
    [[nodiscard]] NomadId getScriptId(const NomadString& name) const;
    [[nodiscard]] Script* getScript(NomadId script_id) const;
    [[nodiscard]] NomadIndex getScriptCount() const;
    void getScripts(std::vector<Script*>& scripts);
    [[nodiscard]] NomadIndex getScriptSize() const;

    std::unique_ptr<Compiler> createCompiler();

    std::unique_ptr<Interpreter> createInterpreter(NomadId scriptId);
    void executeScript(NomadId scriptId);
    void executeScript(NomadId scriptId, ScriptValue& returnValue);

    // Debug
    void dumpInstructions(std::ostream& out);
    void dumpDocumentation(std::ostream& out);

private:
    struct FormatStringRegistration {
        NomadId id;
        NomadId scriptId;
        std::unique_ptr<FormatString> formatString;
    };

    void registerDefaultInstructions();

    std::vector<Instruction> m_instructions;
    std::vector<OpCodeDefinition> m_opCodes;
    std::vector<std::unique_ptr<Type>> m_types;
    std::vector<CommandDefinition> m_commands;
    std::vector<KeywordDefinition> m_keywords;
    VariableMap m_constantsMap;
    std::vector<ScriptValue> m_constants;
    std::vector<NomadString> m_strings;
    std::vector<DynamicVariableRegistration> m_dynamicVariables;
    std::vector<VariableContextRegistration> m_variables;
    std::vector<FormatStringRegistration> m_formatStrings;
    std::vector<Script*> m_scripts;
    bool m_debug = false;
    std::unique_ptr<Interpreter> m_interpreter = nullptr;
};
} // nomad

