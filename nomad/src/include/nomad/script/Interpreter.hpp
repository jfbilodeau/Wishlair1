//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/compiler/Identifier.hpp"

#include "nomad/script/Documentation.hpp"
#include "nomad/script/Script.hpp"
#include "nomad/script/ScriptValue.hpp"

#include <vector>

namespace nomad {

class Interpreter;
class Runtime;
class Script;

struct Instruction {
    explicit Instruction(InstructionFn fn) : fn(fn) {}
    explicit Instruction(const ScriptValue& value) : value(value) {}

    Instruction(const Instruction& other) { fn = other.fn; };

    union {
        InstructionFn fn = nullptr;
        ScriptValue value;
    };
};

class InterpreterException : public NomadException {
public:
    explicit InterpreterException(const NomadString& message) : NomadException(message) {}
};

class Interpreter {
public:
    explicit Interpreter(Runtime* runtime, NomadIndex stack_size = 1024);
    Interpreter(const Interpreter& other) = delete;
    ~Interpreter();

    [[nodiscard]] Runtime* getRuntime() const { return m_runtime; }

    void setScriptVariableValue(NomadId variableId, const ScriptValue& value);
    [[nodiscard]] const ScriptValue& getScriptVariableValue(NomadId variableId) const;

    void run(Script* script, const std::vector<ScriptValue>& parameters);
    void yield(const ScriptValue& result);
    void stop(const ScriptValue& result);
    void fault(const NomadString& faultMessage);

    void callScript(NomadIndex jumpIndex);
    void callCommand(NomadId commandId);
    void returnScript(NomadIndex variableCount);

    void setResult(const ScriptValue& result) { m_result = result; }
    void setIntegerResult(NomadInteger result) { m_result.setIntegerValue(result); }
    void setFloatResult(NomadFloat result) { m_result.setFloatValue(result); }
    void setBooleanResult(NomadBoolean result) { m_result.setBooleanValue(result); }
    void setIdResult(NomadId result) { m_result.setIdValue(result); }
    void setStringResult(const NomadString& result) { m_result.setStringValue(result); }
    void setStringResult(const NomadChar* result) { m_result.setStringValue(result); }

    [[nodiscard]] const ScriptValue& getResult() const { return m_result; }
    [[nodiscard]] NomadInteger getIntegerResult() const { return m_result.getIntegerValue(); }
    [[nodiscard]] NomadFloat getFloatResult() const { return m_result.getFloatValue(); }
    [[nodiscard]] NomadBoolean getBooleanResult() const { return m_result.getBooleanValue(); }
    [[nodiscard]] NomadId getIdResult() const { return m_result.getIdValue(); }
    [[nodiscard]] NomadString getStringResult() const { return m_result.getStringValue(); }

    void setIntermediate(const ScriptValue& intermediate) { m_intermediate = intermediate; }
    void setBooleanIntermediate(NomadBoolean intermediate) { m_intermediate.setBooleanValue(intermediate); }
    void setIntegerIntermediate(NomadInteger intermediate) { m_intermediate.setIntegerValue(intermediate); }
    void setFloatIntermediate(NomadFloat intermediate) { m_intermediate.setFloatValue(intermediate); }
    void setIdIntermediate(NomadId intermediate) { m_intermediate.setIdValue(intermediate); }
    void setStringIntermediate(const NomadString& intermediate) { m_intermediate.setStringValue(intermediate); }

    [[nodiscard]] const ScriptValue& getIntermediate() const { return m_intermediate; }
    [[nodiscard]] NomadBoolean getBooleanIntermediate() const { return m_intermediate.getBooleanValue(); }
    [[nodiscard]] NomadInteger getIntegerIntermediate() const { return m_intermediate.getIntegerValue(); }
    [[nodiscard]] NomadFloat getFloatIntermediate() const { return m_intermediate.getFloatValue(); }
    [[nodiscard]] NomadId getIdIntermediate() const { return m_intermediate.getIdValue(); }
    [[nodiscard]] NomadString getStringIntermediate() const { return m_intermediate.getStringValue(); }

    [[nodiscard]] const ScriptValue& getParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id]; }
    [[nodiscard]] NomadBoolean getBooleanParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id].getBooleanValue(); }
    [[nodiscard]] NomadInteger getIntegerParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id].getIntegerValue(); }
    [[nodiscard]] NomadFloat getFloatParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id].getFloatValue(); }
    [[nodiscard]] NomadId getIdParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id].getIdValue(); }
    [[nodiscard]] const NomadChar* getStringParameter(NomadId parameter_id) const { return m_stack[m_parameterIndex - parameter_id].getStringValue(); }

    void setVariableValue(NomadId variableId, const ScriptValue& value) { m_stack[m_variableIndex - variableId] = value; }
    void setBooleanVariableValue(NomadId variable_id, NomadBoolean value) { m_stack[m_variableIndex - variable_id].setBooleanValue(value); }
    void setIntegerVariableValue(NomadId variable_id, NomadInteger value) { m_stack[m_variableIndex - variable_id].setIntegerValue(value); }
    void setFloatVariableValue(NomadId variable_id, NomadFloat value) { m_stack[m_variableIndex - variable_id].setFloatValue(value); }
    void setIdVariableValue(NomadId variable_id, NomadId value) { m_stack[m_variableIndex - variable_id].setIdValue(value); }
    void setStringVariableValue(NomadId variable_id, const NomadChar* value) { m_stack[m_variableIndex - variable_id].setStringValue(value); }
    void setStringVariableValue(NomadId variable_id, const NomadString& value) { m_stack[m_variableIndex - variable_id].setStringValue(value); }

    void getScriptVariableValue(NomadId variable_id, ScriptValue& value) { value = m_stack[m_variableIndex + variable_id]; }
    [[nodiscard]] NomadBoolean getScriptBooleanVariableValue(NomadId variable_id) { return m_stack[m_variableIndex + variable_id].getBooleanValue(); }
    [[nodiscard]] NomadInteger getScriptIntegerVariableValue(NomadId variable_id) { return m_stack[m_variableIndex + variable_id].getIntegerValue(); }
    [[nodiscard]] NomadFloat getScriptFloatVariableValue(NomadId variable_id) { return m_stack[m_variableIndex + variable_id].getFloatValue(); }
    [[nodiscard]] NomadId getScriptIdVariableValue(NomadId variable_id) { return m_stack[m_variableIndex + variable_id].getIdValue(); }
    [[nodiscard]] const NomadChar* getScriptStringVariableValue(NomadId variable_id) { return m_stack[m_variableIndex + variable_id].getStringValue(); }
    [[nodiscard]] NomadString getScriptStringVariableValue(NomadId variable_id) const { return m_stack[m_variableIndex + variable_id].getStringValue(); }

    void initStringVariable(NomadId variable_id) { m_stack[m_variableIndex - variable_id].initStringValue(); }
    void initStringResult() { m_result.initStringValue(); }
    void initStringIntermediate() { m_intermediate.initStringValue(); }

    void freeStringResult() { m_result.freeStringValue(); }
    void freeStringIntermediate() { m_intermediate.freeStringValue(); }

    void freeStringVariable(NomadIndex variable);
    void freeStringStack(NomadIndex index);

    void pushResult();
    void pushIntermediate();
    void pushStringResult();
    void pushStringIntermediate();

    void popResult();
    void popIntermediate();
    void popStringResult();
    void popStringIntermediate();

    void pushValue(const ScriptValue& value);
    void pushBoolean(NomadBoolean value);
    void pushInteger(NomadInteger value);
    void pushIndex(NomadIndex value);
    void pushFloat(NomadFloat value);
    void pushId(NomadId value);
    void pushString(const NomadString& value);
    void pushOpCodeIndex();

    void pushN(NomadIndex count);
    void pop1();
    void pop2();
    void popN(NomadIndex count);
    const ScriptValue& popValue();
    NomadBoolean popBoolean();
    NomadInteger popInteger();
    NomadIndex popIndex();
    NomadFloat popFloat();
    NomadId popId();
    NomadString popString();
    void popOpCodeIndex();

    NomadId nextInstruction();
    NomadId nextId();
    NomadIndex nextIndex();
    NomadShort nextShort();
    NomadInteger nextInteger();
    NomadFloat nextFloat();

    void jump(NomadIndex index);

    bool getVariableValue(IdentifierType identifier_type, NomadId contextId, NomadId variableId, ScriptValue& value);

    [[nodiscard]] bool isRunning() const { return m_running; }
    [[nodiscard]] bool hasYielded() const { return m_yielded; }

    [[nodiscard]] size_t getInstructionIndex() const { return m_instructionIndex; }

private:
    Runtime* m_runtime;
    NomadIndex m_instructionIndex = 0;
    const std::vector<Instruction>& m_instructions;
    ScriptValue m_result;
    ScriptValue m_intermediate;
    bool m_running = false;
    bool m_yielded = false;
//    std::vector<NomadId> m_op_codes;
    NomadIndex m_stack_index = 0;
    std::vector<ScriptValue> m_stack;
    NomadIndex m_parameterIndex = 0;
    NomadIndex m_variableIndex = 0;
};

} // nomad

