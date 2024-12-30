//
// Created by jbilodeau on 2024-09-17.
//

#include "nomad/script/Interpreter.hpp"

#include "nomad/system/Boolean.hpp"
#include "nomad/system/Float.hpp"
#include "nomad/system/Integer.hpp"

#include "nomad/script/OpCode.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void op_nop(Interpreter* interpreter) {
    // Do nothing...
}

void op_stop(Interpreter* interpreter) {
    interpreter->stop(interpreter->get_result());
}

void op_yield(Interpreter* interpreter) {
    interpreter->yield(interpreter->get_result());
}

void op_return(Interpreter* interpreter) {
    interpreter->return_script(0);
}

void op_return_n(Interpreter* interpreter) {
    auto variable_count = interpreter->next_index();

    interpreter->return_script(variable_count);
}

void op_jump(Interpreter* interpreter) {
    auto jump_index = interpreter->next_index();

    interpreter->jump(jump_index);
}

void op_jump_if(Interpreter* interpreter) {
    auto jump_index = interpreter->next_index();
    auto condition = interpreter->get_boolean_result();

    if (condition) {
        interpreter->jump(jump_index);
    }
}

void op_jump_if_false(Interpreter* interpreter) {
    auto jump_index = interpreter->next_index();
    auto condition = interpreter->get_boolean_result();

    if (!condition) {
        interpreter->jump(jump_index);
    }
}

void op_push_r(Interpreter* interpreter) {
    interpreter->push_result();
}

void op_push_i(Interpreter* interpreter) {
    interpreter->push_intermediate();
}

void op_push_n(Interpreter* interpreter) {
    NomadInteger push_count = interpreter->next_integer();

    interpreter->push_n(push_count);
}

void op_pop_r(Interpreter* interpreter) {
    interpreter->pop_result();
}

void op_pop_i(Interpreter* interpreter) {
    interpreter->pop_intermediate();
}

void op_pop_n(Interpreter* interpreter) {
    NomadInteger pop_count = interpreter->next_integer();

    interpreter->pop_n(pop_count);
}

void op_call_script(Interpreter* interpreter) {
    auto jump_index = interpreter->next_index();

    interpreter->call_script(jump_index);
}

void op_call_command(Interpreter* interpreter) {
    auto command_id = interpreter->next_id();

    interpreter->call_command(command_id);
}

void op_parameter_load_r(Interpreter* interpreter) {
    auto parameter_id = interpreter->next_id();

    auto& value = interpreter->get_parameter(parameter_id);

    interpreter->set_result(value);
}

void op_string_parameter_load_r(Interpreter* interpreter) {
    auto parameter_id = interpreter->next_id();

    auto value = interpreter->get_string_parameter(parameter_id);

    interpreter->set_string_result(value);
}

void op_script_variable_set(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    interpreter->set_script_variable_value(variable_id, interpreter->get_result());
}

void op_script_variable_string_set(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    interpreter->set_string_variable_value(variable_id, interpreter->get_result().get_string_value());
}

void op_script_variable_get(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    ScriptValue value;
    interpreter->get_script_variable_value(variable_id, value);

    interpreter->set_result(value);
}

void op_script_variable_string_get(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    auto value = interpreter->get_script_string_variable_value(variable_id);

    interpreter->set_string_result(value);
}

void op_dynamic_variable_set(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    interpreter->get_runtime()->set_dynamic_variable(interpreter, variable_id, interpreter->get_result());
}

void op_dynamic_variable_string_set(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    interpreter->get_runtime()->set_string_dynamic_variable(interpreter, variable_id, interpreter->get_result());
}

void op_dynamic_variable_get(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    ScriptValue value;

    interpreter->get_runtime()->get_dynamic_variable_value(interpreter, variable_id, value);

    interpreter->set_result(value);
}

void op_dynamic_variable_string_get(Interpreter* interpreter) {
    auto variable_id = interpreter->next_id();

    NomadString value;

    interpreter->get_runtime()->get_string_dynamic_variable_value(interpreter, variable_id, value);

    interpreter->set_string_result(value);
}

void op_context_variable_set(Interpreter* interpreter) {
    auto context_id = interpreter->next_id();
    auto variable_id = interpreter->next_id();

    interpreter->get_runtime()->set_context_variable_value(context_id, variable_id, interpreter->get_result());
}

void op_context_variable_string_set(Interpreter* interpreter) {
    auto context_id = interpreter->next_id();
    auto variable_id = interpreter->next_id();

    interpreter->get_runtime()->set_string_context_variable_value(context_id, variable_id, interpreter->get_result().get_string_value());
}

void op_context_variable_get(Interpreter* interpreter) {
    auto context_id = interpreter->next_id();
    auto variable_id = interpreter->next_id();

    ScriptValue value;

    interpreter->get_runtime()->get_context_variable_value(context_id, variable_id, value);

    interpreter->set_result(value);
}

void op_context_variable_string_get(Interpreter* interpreter) {
    auto context_id = interpreter->next_id();
    auto variable_id = interpreter->next_id();

    NomadString value;

    interpreter->get_runtime()->get_string_context_variable_value(context_id, variable_id, value);

    interpreter->set_string_result(value);
}

void op_boolean_load_false_r(Interpreter* interpreter) {
    interpreter->set_boolean_result(NOMAD_FALSE);
}

void op_boolean_load_false_i(Interpreter* interpreter) {
    interpreter->set_boolean_intermediate(NOMAD_FALSE);
}

void op_boolean_load_true_r(Interpreter* interpreter) {
    interpreter->set_boolean_result(NOMAD_TRUE);
}

void op_boolean_load_true_i(Interpreter* interpreter) {
    interpreter->set_boolean_intermediate(NOMAD_TRUE);
}

void op_boolean_not(Interpreter* interpreter) {
    auto a = interpreter->get_boolean_result();

    auto result = boolean_not(a);

    interpreter->set_boolean_result(result);
}

void op_boolean_and(Interpreter* interpreter) {
    auto a = interpreter->get_boolean_result();
    auto b = interpreter->get_boolean_intermediate();

    auto result = boolean_and(a, b);

    interpreter->set_boolean_result(result);
}

void op_boolean_or(Interpreter* interpreter) {
    auto a = interpreter->get_boolean_result();
    auto b = interpreter->get_boolean_intermediate();

    auto result = boolean_or(a, b);

    interpreter->set_boolean_result(result);
}

void op_boolean_equal(Interpreter* interpreter) {
    auto a = interpreter->get_boolean_result();
    auto b = interpreter->get_boolean_intermediate();

    auto result = boolean_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_boolean_not_equal(Interpreter* interpreter) {
    auto a = interpreter->get_boolean_result();
    auto b = interpreter->get_boolean_intermediate();

    auto result = boolean_not_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_load_zero_r(Interpreter* interpreter) {
    interpreter->set_integer_result(0);
}

void op_integer_load_zero_i(Interpreter* interpreter) {
    interpreter->set_integer_intermediate(0);
}

void op_integer_load_one_r(Interpreter* interpreter) {
    interpreter->set_integer_result(1);
}

void op_integer_load_one_i(Interpreter* interpreter) {
    interpreter->set_integer_intermediate(1);
}

void op_integer_load_r(Interpreter* interpreter) {
    auto value = interpreter->next_integer();

    interpreter->set_integer_result(value);
}

void op_integer_load_i(Interpreter* interpreter) {
    auto value = interpreter->next_integer();

    interpreter->set_integer_intermediate(value);
}

void op_integer_absolute(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();

    auto result = integer_absolute(a);

    interpreter->set_integer_result(result);
}

void op_integer_negate(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();

    auto result = integer_negate(a);

    interpreter->set_integer_result(result);
}

void op_integer_add(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_add(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_subtract(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_subtract(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_multiply(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_multiply(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_divide(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_divide(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_modulo(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_modulo(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_and(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_and(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_or(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_or(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_xor(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_xor(a, b);

    interpreter->set_integer_result(result);
}

void op_integer_equal(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_not_equal(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_not_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_less_than(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_less_than(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_less_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_less_than_or_equal(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_greater_than(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_greater_than(a, b);

    interpreter->set_boolean_result(result);
}

void op_integer_greater_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->get_integer_result();
    auto b = interpreter->get_integer_intermediate();

    auto result = integer_greater_than_or_equal(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_load_zero_r(Interpreter* interpreter) {
    interpreter->set_float_result(0.0);
}

void op_float_load_zero_i(Interpreter* interpreter) {
    interpreter->set_float_intermediate(0.0);
}

void op_float_load_one_r(Interpreter* interpreter) {
    interpreter->set_float_result(1.0);
}

void op_float_load_one_i(Interpreter* interpreter) {
    interpreter->set_float_intermediate(1.0);
}

void op_float_load_r(Interpreter* interpreter) {
    auto value = interpreter->next_float();

    interpreter->set_float_result(value);
}

void op_float_load_i(Interpreter* interpreter) {
    auto value = interpreter->next_float();

    interpreter->set_float_intermediate(value);
}

void op_float_absolute(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();

    auto result = float_absolute(a);

    interpreter->set_float_result(result);
}

void op_float_negate(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();

    auto result = float_negate(a);

    interpreter->set_float_result(result);
}

void op_float_add(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_add(a, b);

    interpreter->set_float_result(result);
}

void op_float_subtract(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_subtract(a, b);

    interpreter->set_float_result(result);
}

void op_float_multiply(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_multiply(a, b);

    interpreter->set_float_result(result);
}

void op_float_divide(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_divide(a, b);

    interpreter->set_float_result(result);
}

void op_float_modulo(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_modulo(a, b);

    interpreter->set_float_result(result);
}

void op_float_sin(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();

    auto result = float_sin(a);

    interpreter->set_float_result(result);
}

void op_float_cosine(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();

    auto result = float_cosine(a);

    interpreter->set_float_result(result);
}

void op_float_tangent(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();

    auto result = float_tangent(a);

    interpreter->set_float_result(result);
}

void op_float_equal(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_not_equal(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_not_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_less_than(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_less_than(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_less_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_less_than_or_equal(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_greater_than(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_greater_than(a, b);

    interpreter->set_boolean_result(result);
}

void op_float_greater_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->get_float_result();
    auto b = interpreter->get_float_intermediate();

    auto result = float_greater_than_or_equal(a, b);

    interpreter->set_boolean_result(result);
}

void op_string_init(Interpreter* interpreter) {
    auto string_index = interpreter->next_index();

    interpreter->init_string_variable(string_index);
}

void op_string_init_r(Interpreter* interpreter) {
    interpreter->init_string_result();
}

void op_string_init_i(Interpreter* interpreter) {
    interpreter->init_string_intermediate();
}

void op_string_free_r(Interpreter* interpreter) {
    interpreter->free_string_result();
}

void op_string_free_i(Interpreter* interpreter) {
    interpreter->free_string_intermediate();
}

void op_string_free_variable(Interpreter* interpreter) {
    auto string_index = interpreter->next_index();

    interpreter->free_string_variable(string_index);
}

void op_string_free_stack(Interpreter* interpreter) {
    auto index = interpreter->next_index();

    interpreter->free_string_stack(index);
}
void op_string_push_r(Interpreter* interpreter) {
    interpreter->push_string_result();
}

void op_string_push_i(Interpreter* interpreter) {
    interpreter->push_string_intermediate();
}

void op_string_pop_r(Interpreter* interpreter) {
    interpreter->pop_string_result();
}

void op_string_pop_i(Interpreter* interpreter) {
    interpreter->pop_string_intermediate();
}

void op_string_load_r(Interpreter* interpreter) {
    auto string_id = interpreter->next_id();

    auto& string = interpreter->get_runtime()->get_string(string_id);

    interpreter->set_string_result(string);
}

void op_string_load_i(Interpreter* interpreter) {
    auto string_id = interpreter->next_id();

    auto string = interpreter->get_runtime()->get_string(string_id);

    interpreter->set_string_intermediate(string);
}

void op_string_equal(Interpreter* interpreter) {
    auto a = interpreter->get_result().get_string_value();
    auto b = interpreter->get_intermediate().get_string_value();

    auto result = string_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_string_not_equal(Interpreter* interpreter) {
    auto a = interpreter->get_string_result();
    auto b = interpreter->get_string_intermediate();

    auto result = string_not_equal_to(a, b);

    interpreter->set_boolean_result(result);
}

void op_format_string_execute(Interpreter* interpreter) {
    auto format_string_id = interpreter->next_id();

    auto format_string = interpreter->get_runtime()->get_format_string(format_string_id);

    auto string = format_string->format(interpreter);

    interpreter->set_string_result(string);
}

void op_id_push(Interpreter* interpreter) {
    auto id = interpreter->next_id();

    interpreter->push_id(id);
}

void op_id_load_r(Interpreter* interpreter) {
    auto id = interpreter->next_id();

    interpreter->set_id_result(id);
}

void op_id_load_i(Interpreter* interpreter) {
    auto id = interpreter->next_id();

    interpreter->set_id_intermediate(id);
}

void op_assert(Interpreter* interpreter) {
    auto assert_result = interpreter->get_boolean_result();
    auto assert_message_id = interpreter->get_id_intermediate();

    // Bitwise comparison
    if (assert_result) {
        auto& assertion_message = interpreter->get_runtime()->get_string(assert_message_id);

        log::error(assertion_message);

        interpreter->fault(assertion_message);
    }
}

} // namespace nomad
