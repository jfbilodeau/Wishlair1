//
// Created by jfbil on 2023-06-04.
//

#include "nomad/script/Runtime.hpp"
#include "nomad/script/Script.hpp"

#include "nomad/script/Interpreter.hpp"

#include "nomad/system/String.hpp"

#ifdef NOMAD_DEBUG
#define TEST_STACK_OVERFLOW if (m_stack_index >= m_stack.size()) { fault("Stack overflow"); }
#define TEST_STACK_UNDERFLOW if (m_stack_index >= m_stack.size()) { fault("Stack underflow"); }
#else
#define TEST_STACK_OVERFLOW
#define TEST_STACK_UNDERFLOW
#endif

namespace nomad {

Interpreter::Interpreter(Runtime* runtime, NomadIndex stack_size):
    m_runtime(runtime),
    m_instructions(runtime->getInstructions())
{
    m_stack.resize(stack_size);
}

Interpreter::~Interpreter() = default;

void Interpreter::setScriptVariableValue(NomadId variableId, const ScriptValue& value) {
    m_stack[m_variableIndex + variableId] = value;
}

const ScriptValue& Interpreter::getScriptVariableValue(NomadId variableId) const {
    return m_stack[m_variableIndex + variableId];
}

void Interpreter::run(Script* script, const std::vector<ScriptValue>& parameters) {
#ifdef NOMAD_DEBUG
//    log::debug("Running script: " + script->get_name());
//    log::flush();
#endif

    m_running = true;
    m_yielded = false;

    for (auto parameter = parameters.rbegin(); parameter != parameters.rend(); parameter++) {
        pushValue(*parameter);
    }

    callScript(script->getScriptStart());

    do {
#ifdef NOMAD_DEBUG
//        auto fn = m_instructions[m_instruction_index].fn;
//        auto id = m_runtime->get_instruction_id(fn);
//        auto& name = m_runtime->get_instruction_name(id);
//        log::debug("Executing: " + name + "(ip: " + to_string(m_instruction_index) + ", sp: " + to_string(m_stack_index) + ")");
//        log::flush();
#endif
        auto& instruction = m_instructions[m_instructionIndex];

        m_instructionIndex++;

        instruction.fn(this);
    } while (m_running);

    m_running = false;

    if (!m_yielded) {
        m_instructionIndex = 0;
    }

    // Free parameters
//    for (auto i = 0; i < parameters.size(); i++) {
//        auto type = script->get_parameter_type(i);
//        type->free_value(m_stack[m_parameter_index - i]);
//    }

//    return_script(script->get_variable_count());
}

void Interpreter::yield(const ScriptValue& result) {
    m_result = result;

    m_running = false;
    m_yielded = true;
}

void Interpreter::stop(const ScriptValue& result) {
    m_result = result;

    m_running = false;
    m_yielded = false;
}

void Interpreter::fault(const NomadString& faultMessage) {
    throw InterpreterException(faultMessage);
}

Script * Interpreter::getCurrentScript() const {
    auto tempScripts = createTempVector<Script*>();

    m_runtime->getScripts(tempScripts);

    for (auto script : tempScripts) {
        if (m_instructionIndex >= script->getScriptStart() && m_instructionIndex <= script->getScriptEnd()) {
            return script;
        }
    }

    return nullptr;
}

void Interpreter::callScript(NomadIndex jumpIndex) {
    auto previous_parameter_index = m_parameterIndex;
    m_parameterIndex = m_stack_index;

    pushIndex(m_instructionIndex);
    pushIndex(previous_parameter_index);
    pushIndex(m_variableIndex);

    m_variableIndex = m_stack_index;

    jump(jumpIndex);
}

void Interpreter::callCommand(NomadId commandId) {
    auto previous_parameter_index = m_parameterIndex;
    m_parameterIndex = m_stack_index;

    pushIndex(previous_parameter_index);

    auto command_fn = m_runtime->getCommandFn(commandId);

#ifdef NOMAD_DEBUG
    if (command_fn == nullptr) {
        log::error("Command not found: " + toString(commandId));
        return;
    }
#endif

    command_fn(this);

    m_parameterIndex = popIndex();
}

void Interpreter::returnScript(NomadIndex variableCount) {
    popN(variableCount);

    m_variableIndex = popIndex();
    m_parameterIndex = popIndex();
    m_instructionIndex = popIndex();
}

void Interpreter::pushResult() {
    m_stack_index++;
    m_stack[m_stack_index] = m_result;
    TEST_STACK_OVERFLOW
}

void Interpreter::pushIntermediate() {
    m_stack_index++;
    m_stack[m_stack_index] = m_intermediate;
    TEST_STACK_OVERFLOW
}

void Interpreter::pushStringResult() {
    m_stack_index++;
    m_stack[m_stack_index].move_string_value(m_result);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushStringIntermediate() {
    m_stack_index++;
    m_stack[m_stack_index].move_string_value(m_intermediate);
    TEST_STACK_OVERFLOW
}

void Interpreter::freeStringVariable(NomadIndex variable_index) {
    m_stack[m_variableIndex + variable_index].freeStringValue();
}

void Interpreter::freeStringStack(NomadIndex index) {
    m_stack[m_stack_index - index].freeStringValue();
}

void Interpreter::popResult() {
    m_result = m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::popIntermediate() {
    m_intermediate = m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::popStringResult() {
    m_result.move_string_value(m_stack[m_stack_index]);
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::popStringIntermediate() {
    m_intermediate.move_string_value(m_stack[m_stack_index]);
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}


void Interpreter::pushValue(const ScriptValue& value) {
    m_stack_index++;
    m_stack[m_stack_index] = value;
    TEST_STACK_OVERFLOW
}

void Interpreter::pushBoolean(NomadBoolean value) {
    m_stack_index++;
    m_stack[m_stack_index].setBooleanValue(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushInteger(NomadInteger value) {
    m_stack_index++;
    m_stack[m_stack_index].setIntegerValue(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushFloat(NomadFloat value) {
    m_stack_index++;
    m_stack[m_stack_index].setFloatValue(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushId(NomadId value) {
    m_stack_index++;
    m_stack[m_stack_index].setIdValue(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushString(const NomadString& value) {
    m_stack_index++;
    m_stack[m_stack_index].setStringValue(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::pushN(NomadIndex count) {
    m_stack_index += count;
    TEST_STACK_OVERFLOW
}

void Interpreter::pop1() {
    m_stack_index--;
    TEST_STACK_OVERFLOW
}

void Interpreter::pop2() {
    m_stack_index -= 2;
    TEST_STACK_OVERFLOW
}

void Interpreter::popN(NomadIndex count) {
    m_stack_index -= count;
    TEST_STACK_UNDERFLOW
}

const ScriptValue& Interpreter::popValue() {
    auto& value= m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW

    return value;
}

NomadBoolean Interpreter::popBoolean() {
    auto value = m_stack[m_stack_index].getBooleanValue();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadInteger Interpreter::popInteger() {
    auto value = m_stack[m_stack_index].getIntegerValue();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadFloat Interpreter::popFloat() {
    auto value = m_stack[m_stack_index].getFloatValue();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadId Interpreter::popId() {
    auto value = m_stack[m_stack_index].getIdValue();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadString Interpreter::popString() {
    auto value = m_stack[m_stack_index].getStringValue();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadId Interpreter::nextId() {
    auto id = m_instructions[m_instructionIndex].value.getIdValue();

    m_instructionIndex++;

    return id;
}

NomadIndex Interpreter::nextIndex() {
    auto index = m_instructions[m_instructionIndex].value.get_index_value();

    m_instructionIndex++;

    return index;
}

NomadInteger Interpreter::nextInteger() {
    const auto integer_value = m_instructions[m_instructionIndex].value.getIntegerValue();

    m_instructionIndex++;

    return integer_value;
}

NomadFloat Interpreter::nextFloat() {
    const auto float_value = m_instructions[m_instructionIndex].value.getFloatValue();

    m_instructionIndex++;

    return float_value;
}

void Interpreter::jump(NomadIndex index) {
    m_instructionIndex = index;
}

bool Interpreter::getVariableValue(IdentifierType identifier_type, NomadId contextId, NomadId variableId, ScriptValue& value) {
    switch (identifier_type) {
    case IdentifierType::Constant:
        m_runtime->getConstantValue(variableId, value);
        return true;

    case IdentifierType::DynamicVariable:
        m_runtime->getDynamicVariableValue(this, variableId, value);
        return true;

    case IdentifierType::ContextVariable:
        m_runtime->getContextVariableValue(contextId, variableId, value);
        return true;

    case IdentifierType::ScriptVariable:
        getScriptVariableValue(variableId, value);
        return true;

    case IdentifierType::Parameter:
        value = getParameter(variableId);
        return true;

    default:
        log::warning("Unexpected identifier type: " + toString(static_cast<int>(identifier_type)));
    }

    return false;
}

void Interpreter::pushIndex(NomadIndex value) {
    m_stack_index++;
    m_stack[m_stack_index].setIndexValue(value);
    TEST_STACK_OVERFLOW
}

NomadIndex Interpreter::popIndex() {
    auto value = m_stack[m_stack_index].get_index_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

void Interpreter::pushOpCodeIndex() {
    m_stack_index++;
    m_stack[m_stack_index].setIndexValue(m_instructionIndex);
    TEST_STACK_OVERFLOW
}

void Interpreter::popOpCodeIndex() {
    m_instructionIndex = m_stack[m_stack_index].get_index_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

} // nomad