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
    m_instructions(runtime->get_instructions())
{
    m_stack.resize(stack_size);
}

Interpreter::~Interpreter() = default;

void Interpreter::set_script_variable_value(NomadId variable_id, const ScriptValue& value) {
    m_stack[m_variable_index + variable_id] = value;
}

const ScriptValue& Interpreter::get_script_variable_value(NomadId variable_id) const {
    return m_stack[m_variable_index + variable_id];
}

void Interpreter::run(Script* script, const std::vector<ScriptValue>& parameters) {
#ifdef NOMAD_DEBUG
//    log::debug("Running script: " + script->get_name());
//    log::flush();
#endif

    m_running = true;
    m_yielded = false;

    for (auto parameter = parameters.rbegin(); parameter != parameters.rend(); parameter++) {
        push_value(*parameter);
    }

    call_script(script->get_script_start());

    do {
#ifdef NOMAD_DEBUG
//        auto fn = m_instructions[m_instruction_index].fn;
//        auto id = m_runtime->get_instruction_id(fn);
//        auto& name = m_runtime->get_instruction_name(id);
//        log::debug("Executing: " + name + "(ip: " + to_string(m_instruction_index) + ", sp: " + to_string(m_stack_index) + ")");
//        log::flush();
#endif
        auto& instruction = m_instructions[m_instruction_index];

        m_instruction_index++;

        instruction.fn(this);
    } while (m_running);

    m_running = false;

    if (!m_yielded) {
        m_instruction_index = 0;
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

void Interpreter::fault(const NomadString& fault_message) {
    throw InterpreterException(fault_message);
}

void Interpreter::call_script(NomadIndex jump_index) {
    auto previous_parameter_index = m_parameter_index;
    m_parameter_index = m_stack_index;

    push_index(m_instruction_index);
    push_index(previous_parameter_index);
    push_index(m_variable_index);

    m_variable_index = m_stack_index;

    jump(jump_index);
}

void Interpreter::call_command(NomadId command_id) {
    auto previous_parameter_index = m_parameter_index;
    m_parameter_index = m_stack_index;

    push_index(previous_parameter_index);

    auto command_fn = m_runtime->get_command_fn(command_id);

#ifdef NOMAD_DEBUG
    if (command_fn == nullptr) {
        log::error("Command not found: " + to_string(command_id));
        return;
    }
#endif

    command_fn(this);

    m_parameter_index = pop_index();
}

void Interpreter::return_script(NomadIndex variable_count) {
    pop_n(variable_count);

    m_variable_index = pop_index();
    m_parameter_index = pop_index();
    m_instruction_index = pop_index();
}

void Interpreter::push_result() {
    m_stack_index++;
    m_stack[m_stack_index] = m_result;
    TEST_STACK_OVERFLOW
}

void Interpreter::push_intermediate() {
    m_stack_index++;
    m_stack[m_stack_index] = m_intermediate;
    TEST_STACK_OVERFLOW
}

void Interpreter::push_string_result() {
    m_stack_index++;
    m_stack[m_stack_index].move_string_value(m_result);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_string_intermediate() {
    m_stack_index++;
    m_stack[m_stack_index].move_string_value(m_intermediate);
    TEST_STACK_OVERFLOW
}

void Interpreter::free_string_variable(NomadIndex variable_index) {
    m_stack[m_variable_index + variable_index].free_string_value();
}

void Interpreter::free_string_stack(NomadIndex index) {
    m_stack[m_stack_index - index].free_string_value();
}

void Interpreter::pop_result() {
    m_result = m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::pop_intermediate() {
    m_intermediate = m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::pop_string_result() {
    m_result.move_string_value(m_stack[m_stack_index]);
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

void Interpreter::pop_string_intermediate() {
    m_intermediate.move_string_value(m_stack[m_stack_index]);
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}


void Interpreter::push_value(const ScriptValue& value) {
    m_stack_index++;
    m_stack[m_stack_index] = value;
    TEST_STACK_OVERFLOW
}

void Interpreter::push_boolean(NomadBoolean value) {
    m_stack_index++;
    m_stack[m_stack_index].set_boolean_value(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_integer(NomadInteger value) {
    m_stack_index++;
    m_stack[m_stack_index].set_integer_value(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_float(NomadFloat value) {
    m_stack_index++;
    m_stack[m_stack_index].set_float_value(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_id(NomadId value) {
    m_stack_index++;
    m_stack[m_stack_index].set_id_value(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_string(const NomadString& value) {
    m_stack_index++;
    m_stack[m_stack_index].set_string_value(value);
    TEST_STACK_OVERFLOW
}

void Interpreter::push_n(NomadIndex count) {
    m_stack_index += count;
    TEST_STACK_OVERFLOW
}

void Interpreter::pop_1() {
    m_stack_index--;
    TEST_STACK_OVERFLOW
}

void Interpreter::pop_2() {
    m_stack_index -= 2;
    TEST_STACK_OVERFLOW
}

void Interpreter::pop_n(NomadIndex count) {
    m_stack_index -= count;
    TEST_STACK_UNDERFLOW
}

const ScriptValue& Interpreter::pop_value() {
    auto& value= m_stack[m_stack_index];
    m_stack_index--;
    TEST_STACK_UNDERFLOW

    return value;
}

NomadBoolean Interpreter::pop_boolean() {
    auto value = m_stack[m_stack_index].get_boolean_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadInteger Interpreter::pop_integer() {
    auto value = m_stack[m_stack_index].get_integer_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadFloat Interpreter::pop_float() {
    auto value = m_stack[m_stack_index].get_float_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadId Interpreter::pop_id() {
    auto value = m_stack[m_stack_index].get_id_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadString Interpreter::pop_string() {
    auto value = m_stack[m_stack_index].get_string_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

NomadId Interpreter::next_id() {
    auto id = m_instructions[m_instruction_index].value.get_id_value();

    m_instruction_index++;

    return id;
}

NomadIndex Interpreter::next_index() {
    auto index = m_instructions[m_instruction_index].value.get_index_value();

    m_instruction_index++;

    return index;
}

NomadInteger Interpreter::next_integer() {
    const auto integer_value = m_instructions[m_instruction_index].value.get_integer_value();

    m_instruction_index++;

    return integer_value;
}

NomadFloat Interpreter::next_float() {
    const auto float_value = m_instructions[m_instruction_index].value.get_float_value();

    m_instruction_index++;

    return float_value;
}

void Interpreter::jump(NomadIndex index) {
    m_instruction_index = index;
}

bool Interpreter::get_variable_value(IdentifierType identifier_type, NomadId context_id, NomadId variable_id, ScriptValue& value) {
    switch (identifier_type) {
    case IdentifierType::Constant:
        m_runtime->get_constant_value(variable_id, value);
        return true;

    case IdentifierType::DynamicVariable:
        m_runtime->get_dynamic_variable_value(this, variable_id, value);
        return true;

    case IdentifierType::ContextVariable:
        m_runtime->get_context_variable_value(context_id, variable_id, value);
        return true;

    case IdentifierType::ScriptVariable:
        get_script_variable_value(variable_id, value);
        return true;

    case IdentifierType::Parameter:
        value = get_parameter(variable_id);
        return true;

    default:
        log::warning("Unexpected identifier type: " + std::to_string(static_cast<int>(identifier_type)));
    }

    return false;
}

void Interpreter::push_index(NomadIndex value) {
    m_stack_index++;
    m_stack[m_stack_index].set_index_value(value);
    TEST_STACK_OVERFLOW
}

NomadIndex Interpreter::pop_index() {
    auto value = m_stack[m_stack_index].get_index_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
    return value;
}

void Interpreter::push_op_code_index() {
    m_stack_index++;
    m_stack[m_stack_index].set_index_value(m_instruction_index);
    TEST_STACK_OVERFLOW
}

void Interpreter::pop_op_code_index() {
    m_instruction_index = m_stack[m_stack_index].get_index_value();
    m_stack_index--;
    TEST_STACK_UNDERFLOW
}

} // nomad