//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_INTERPRETER_HPP
#define NOMAD_INTERPRETER_HPP

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

    [[nodiscard]] Runtime* get_runtime() const { return m_runtime; }

    void set_script_variable_value(NomadId variable_id, const ScriptValue& value);
    [[nodiscard]] const ScriptValue& get_script_variable_value(NomadId variable_id) const;

    void run(Script* script, const std::vector<ScriptValue>& parameters);
    void yield(const ScriptValue& result);
    void stop(const ScriptValue& result);
    void fault(const NomadString& fault_message);

    void call_script(NomadIndex jump_index);
    void call_command(NomadId command_id);
    void return_script(NomadIndex variable_count);

    void set_result(const ScriptValue& result) { m_result = result; }
    void set_integer_result(NomadInteger result) { m_result.set_integer_value(result); }
    void set_float_result(NomadFloat result) { m_result.set_float_value(result); }
    void set_boolean_result(NomadBoolean result) { m_result.set_boolean_value(result); }
    void set_id_result(NomadId result) { m_result.set_id_value(result); }
    void set_string_result(const NomadString& result) { m_result.set_string_value(result); }
    void set_string_result(const NomadChar* result) { m_result.set_string_value(result); }

    [[nodiscard]] const ScriptValue& get_result() const { return m_result; }
    [[nodiscard]] NomadInteger get_integer_result() const { return m_result.get_integer_value(); }
    [[nodiscard]] NomadFloat get_float_result() const { return m_result.get_float_value(); }
    [[nodiscard]] NomadBoolean get_boolean_result() const { return m_result.get_boolean_value(); }
    [[nodiscard]] NomadId get_id_result() const { return m_result.get_id_value(); }
    [[nodiscard]] NomadString get_string_result() const { return m_result.get_string_value(); }

    void set_intermediate(const ScriptValue& intermediate) { m_intermediate = intermediate; }
    void set_boolean_intermediate(NomadBoolean intermediate) { m_intermediate.set_boolean_value(intermediate); }
    void set_integer_intermediate(NomadInteger intermediate) { m_intermediate.set_integer_value(intermediate); }
    void set_float_intermediate(NomadFloat intermediate) { m_intermediate.set_float_value(intermediate); }
    void set_id_intermediate(NomadId intermediate) { m_intermediate.set_id_value(intermediate); }
    void set_string_intermediate(const NomadString& intermediate) { m_intermediate.set_string_value(intermediate); }

    [[nodiscard]] const ScriptValue& get_intermediate() const { return m_intermediate; }
    [[nodiscard]] NomadBoolean get_boolean_intermediate() const { return m_intermediate.get_boolean_value(); }
    [[nodiscard]] NomadInteger get_integer_intermediate() const { return m_intermediate.get_integer_value(); }
    [[nodiscard]] NomadFloat get_float_intermediate() const { return m_intermediate.get_float_value(); }
    [[nodiscard]] NomadId get_id_intermediate() const { return m_intermediate.get_id_value(); }
    [[nodiscard]] NomadString get_string_intermediate() const { return m_intermediate.get_string_value(); }

    [[nodiscard]] const ScriptValue& get_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id]; }
    [[nodiscard]] NomadBoolean get_boolean_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id].get_boolean_value(); }
    [[nodiscard]] NomadInteger get_integer_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id].get_integer_value(); }
    [[nodiscard]] NomadFloat get_float_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id].get_float_value(); }
    [[nodiscard]] NomadId get_id_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id].get_id_value(); }
    [[nodiscard]] const NomadChar* get_string_parameter(NomadId parameter_id) const { return m_stack[m_parameter_index - parameter_id].get_string_value(); }

    void set_variable_value(NomadId variable_id, const ScriptValue& value) { m_stack[m_variable_index - variable_id] = value; }
    void set_boolean_variable_value(NomadId variable_id, NomadBoolean value) { m_stack[m_variable_index - variable_id].set_boolean_value(value); }
    void set_integer_variable_value(NomadId variable_id, NomadInteger value) { m_stack[m_variable_index - variable_id].set_integer_value(value); }
    void set_float_variable_value(NomadId variable_id, NomadFloat value) { m_stack[m_variable_index - variable_id].set_float_value(value); }
    void set_id_variable_value(NomadId variable_id, NomadId value) { m_stack[m_variable_index - variable_id].set_id_value(value); }
    void set_string_variable_value(NomadId variable_id, const NomadChar* value) { m_stack[m_variable_index - variable_id].set_string_value(value); }
    void set_string_variable_value(NomadId variable_id, const NomadString& value) { m_stack[m_variable_index - variable_id].set_string_value(value); }

    void get_script_variable_value(NomadId variable_id, ScriptValue& value) { value = m_stack[m_variable_index + variable_id]; }
    [[nodiscard]] NomadBoolean get_script_boolean_variable_value(NomadId variable_id) { return m_stack[m_variable_index + variable_id].get_boolean_value(); }
    [[nodiscard]] NomadInteger get_script_integer_variable_value(NomadId variable_id) { return m_stack[m_variable_index + variable_id].get_integer_value(); }
    [[nodiscard]] NomadFloat get_script_float_variable_value(NomadId variable_id) { return m_stack[m_variable_index + variable_id].get_float_value(); }
    [[nodiscard]] NomadId get_script_id_variable_value(NomadId variable_id) { return m_stack[m_variable_index + variable_id].get_id_value(); }
    [[nodiscard]] const NomadChar* get_script_string_variable_value(NomadId variable_id) { return m_stack[m_variable_index + variable_id].get_string_value(); }
    [[nodiscard]] NomadString get_script_string_variable_value(NomadId variable_id) const { return m_stack[m_variable_index + variable_id].get_string_value(); }

    void init_string_variable(NomadId variable_id) { m_stack[m_variable_index - variable_id].init_string_value(); }
    void init_string_result() { m_result.init_string_value(); }
    void init_string_intermediate() { m_intermediate.init_string_value(); }

    void free_string_result() { m_result.free_string_value(); }
    void free_string_intermediate() { m_intermediate.free_string_value(); }

    void free_string_variable(NomadIndex variable);
    void free_string_stack(NomadIndex index);

    void push_result();
    void push_intermediate();
    void push_string_result();
    void push_string_intermediate();

    void pop_result();
    void pop_intermediate();
    void pop_string_result();
    void pop_string_intermediate();

    void push_value(const ScriptValue& value);
    void push_boolean(NomadBoolean value);
    void push_integer(NomadInteger value);
    void push_index(NomadIndex value);
    void push_float(NomadFloat value);
    void push_id(NomadId value);
    void push_string(const NomadString& value);
    void push_op_code_index();

    void push_n(NomadIndex count);
    void pop_1();
    void pop_2();
    void pop_n(NomadIndex count);
    const ScriptValue& pop_value();
    NomadBoolean pop_boolean();
    NomadInteger pop_integer();
    NomadIndex pop_index();
    NomadFloat pop_float();
    NomadId pop_id();
    NomadString pop_string();
    void pop_op_code_index();

    NomadId next_instruction();
    NomadId next_id();
    NomadIndex next_index();
    NomadShort next_short();
    NomadInteger next_integer();
    NomadFloat next_float();

    void jump(NomadIndex index);

    bool get_variable_value(IdentifierType identifier_type, NomadId context_id, NomadId variable_id, ScriptValue& value);

    [[nodiscard]] bool is_running() const { return m_running; }
    [[nodiscard]] bool has_yielded() const { return m_yielded; }

    [[nodiscard]] size_t get_instruction_index() const { return m_instruction_index; }

private:
    Runtime* m_runtime;
    NomadIndex m_instruction_index = 0;
    const std::vector<Instruction>& m_instructions;
    ScriptValue m_result;
    ScriptValue m_intermediate;
    bool m_running = false;
    bool m_yielded = false;
//    std::vector<NomadId> m_op_codes;
    NomadIndex m_stack_index = 0;
    std::vector<ScriptValue> m_stack;
    NomadIndex m_parameter_index = 0;
    NomadIndex m_variable_index = 0;
};

} // nomad

#endif //NOMAD_INTERPRETER_HPP
