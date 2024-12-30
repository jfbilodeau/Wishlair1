//
// Created by jfbil on 2024-10-19.
//

#include "nomad/script/Runtime.hpp"

namespace nomad {

void Runtime::register_default_instructions() {
    // OpCode definitions
    register_instruction(OpCodes::op_nop, op_nop, "No operation", {});
    register_instruction(OpCodes::op_stop, op_stop, "Stop the execution of the interpreter", {});
    register_instruction(OpCodes::op_yield, op_yield, "Yield the execution of the interpreter", {});
    register_instruction(OpCodes::op_return, op_return, "Return from the current script. Does not free variables from the stack", {});
    register_instruction(
        OpCodes::op_return_n, op_return_n, "Return from the current script. Frees `n` entry from the stack",
        {Operand::Integer}
    );

    // Flow control
    register_instruction(OpCodes::op_jump, op_jump, "Jump to the given index (address)", {Operand::Integer});
    register_instruction(
        OpCodes::op_jump_if, op_jump_if, "Jump to the given index (address) if the `r` register is true",
        {Operand::Integer}
    );
    register_instruction(
        OpCodes::op_jump_if_false, op_jump_if_false, "Jump to the given index (address) if the `r` register is false",
        {Operand::Integer}
    );

    // Register push and pop
    register_instruction(OpCodes::op_push_r, op_push_r, "Push the `r` register value to the stack", {});
    register_instruction(OpCodes::op_push_i, op_push_i, "Push the `i` register value to the stack", {});
    register_instruction(
        OpCodes::op_push_n, op_push_n, "Reserve `n` spaces on the stack for script variables", {Operand::Integer}
    );
    register_instruction(OpCodes::op_pop_r, op_pop_r, "Pop the stack value to the `r` register", {});
    register_instruction(OpCodes::op_pop_i, op_pop_i, "Pop the stack value to the `i` register", {});
    register_instruction(OpCodes::op_pop_n, op_pop_n, "Pop (free) `n` values from the stack", {Operand::Integer});

    // Script and command calls
    register_instruction(
        OpCodes::op_call_script, op_call_script,
        "Allocate `n` variables on the stack and call the script at the given index (address)",
        {Operand::Integer}
    );
    register_instruction(OpCodes::op_call_command, op_call_command, "Call the given command id", {Operand::Command});
    register_instruction(OpCodes::op_parameter_load_r, op_parameter_load_r, "Load the `n`th parameter into result", {Operand::Id});
    register_instruction(OpCodes::op_string_parameter_load_r, op_string_parameter_load_r, "Load the `n`th string parameter into result", {Operand::Id});

    // Variable operations
    register_instruction(
        OpCodes::op_script_variable_set, op_script_variable_set,
        "Set the `id` script variable from the `r` register value", {Operand::ScriptVariable}
    );
    register_instruction(
        OpCodes::op_script_variable_string_set, op_script_variable_string_set,
        "Set the `id` string script variable from the `s` register value", {Operand::ScriptVariable}
    );
    register_instruction(
        OpCodes::op_script_variable_get, op_script_variable_get,
        "Load the `id` script variable value to the `r` register", {Operand::ScriptVariable}
    );
    register_instruction(
        OpCodes::op_script_variable_string_get, op_script_variable_string_get,
        "Load the `id` string script variable value to the `s` register", {Operand::ScriptVariable}
    );
    register_instruction(
        OpCodes::op_dynamic_variable_set, op_dynamic_variable_set,
        "Set the dynamic variable `id` from the `r` register value", {Operand::DynamicVariable}
    );
    register_instruction(
        OpCodes::op_dynamic_variable_string_set, op_dynamic_variable_string_set,
        "Set the string dynamic variable `id` from the `s` register value", {Operand::DynamicVariable}
    );
    register_instruction(
        OpCodes::op_dynamic_variable_get, op_dynamic_variable_get,
        "Load the dynamic variable `id` value to the `r` register", {Operand::DynamicVariable}
    );
    register_instruction(
        OpCodes::op_dynamic_variable_string_get, op_dynamic_variable_string_get,
        "Load the dynamic string variable `id` value to the `s` register", {Operand::DynamicVariable}
    );
    register_instruction(
        OpCodes::op_context_variable_set, op_context_variable_set,
        "Set the context variable `context_id`, `variable_id` from the `r` register value",
        {Operand::ContextId, Operand::ContextVariableId}
    );
    register_instruction(
        OpCodes::op_context_variable_string_set, op_context_variable_string_set,
        "Set the context string variable `id` from the `s` register value",
        {Operand::ContextId, Operand::ContextVariableId}
    );
    register_instruction(
        OpCodes::op_context_variable_get, op_context_variable_get,
        "Load the context variable `id` value to the `r` register", {Operand::ContextId, Operand::ContextVariableId}
    );
    register_instruction(
        OpCodes::op_context_variable_string_get, op_context_variable_string_get,
        "Load the string context variable `id` value to the `s` register",
        {Operand::ContextId, Operand::ContextVariableId}
    );

    // Boolean operators
    register_instruction(
        OpCodes::op_boolean_load_false_r, op_boolean_load_false_r, "Load the boolean false value to the `r` register",
        {}
    );
    register_instruction(
        OpCodes::op_boolean_load_false_i, op_boolean_load_false_i, "Load the boolean false value to the `i` register",
        {}
    );
    register_instruction(
        OpCodes::op_boolean_load_true_r, op_boolean_load_true_r, "Load the boolean true value to the `r` register", {}
    );
    register_instruction(
        OpCodes::op_boolean_load_true_i, op_boolean_load_true_i, "Load the boolean true value to the `i` register", {}
    );
    register_instruction(OpCodes::op_boolean_not, op_boolean_not, "Negate the `r` register boolean value", {});
    register_instruction(
        OpCodes::op_boolean_and, op_boolean_and, "Logical AND the `r` and `i` register boolean values", {}
    );
    register_instruction(
        OpCodes::op_boolean_or, op_boolean_or, "Logical OR the `r` and `i` register boolean values", {}
    );
    register_instruction(
        OpCodes::op_boolean_equal, op_boolean_equal, "Test if the `r` and `i` register boolean values are equal", {}
    );
    register_instruction(
        OpCodes::op_boolean_not_equal, op_boolean_not_equal,
        "Test if the `r` and `i` register boolean values are not equal", {}
    );


    // Integer operators
    register_instruction(
        OpCodes::op_integer_load_zero_r, op_integer_load_zero_r, "Load the integer zero value to the `r` register", {}
    );
    register_instruction(
        OpCodes::op_integer_load_zero_i, op_integer_load_zero_i, "Load the integer zero value to the `i` register", {}
    );
    register_instruction(
        OpCodes::op_integer_load_one_r, op_integer_load_one_r, "Load the integer one value to the `r` register", {}
    );
    register_instruction(
        OpCodes::op_integer_load_one_i, op_integer_load_one_i, "Load the integer one value to the `i` register", {}
    );
    register_instruction(
        OpCodes::op_integer_load_r, op_integer_load_r, "Load the integer value to the `r` register",
        {Operand::Integer}
    );
    register_instruction(
        OpCodes::op_integer_load_i, op_integer_load_i, "Load the integer value to the `i` register",
        {Operand::Integer}
    );
    register_instruction(
        OpCodes::op_integer_absolute, op_integer_absolute, "Get the absolute value of the `r` register", {}
    );
    register_instruction(OpCodes::op_integer_negate, op_integer_negate, "Negate the `r` register integer value", {});
    register_instruction(OpCodes::op_integer_add, op_integer_add, "Add the `r` and `i` register integer values", {});
    register_instruction(
        OpCodes::op_integer_subtract, op_integer_subtract, "Subtract the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_multiply, op_integer_multiply, "Multiply the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_divide, op_integer_divide, "Divide the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_modulo, op_integer_modulo, "Modulo the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_and, op_integer_and, "Bitwise AND the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_or, op_integer_or, "Bitwise OR the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_xor, op_integer_xor, "Bitwise XOR the `r` and `i` register integer values", {}
    );
    register_instruction(
        OpCodes::op_integer_equal, op_integer_equal, "Test if the `r` and `i` register integer values are equal", {}
    );
    register_instruction(
        OpCodes::op_integer_not_equal, op_integer_not_equal,
        "Test if the `r` and `i` register integer values are not equal", {}
    );
    register_instruction(
        OpCodes::op_integer_less_than, op_integer_less_than,
        "Test if the `r` register integer value is less than the `i` register integer value", {}
    );
    register_instruction(
        OpCodes::op_integer_less_than_or_equal, op_integer_less_than_or_equal,
        "Test if the `r` register integer value is less than or equal to the `i` register integer value", {}
    );
    register_instruction(
        OpCodes::op_integer_greater_than, op_integer_greater_than,
        "Test if the `r` register integer value is greater than the `i` register integer value", {}
    );
    register_instruction(
        OpCodes::op_integer_greater_than_or_equal, op_integer_greater_than_or_equal,
        "Test if the `r` register integer value is greater than or equal to the `i` register integer value", {}
    );


    // Float operators
    register_instruction(
        OpCodes::op_float_load_zero_r, op_float_load_zero_r, "Load the float zero value to the `r` register", {}
    );
    register_instruction(
        OpCodes::op_float_load_zero_i, op_float_load_zero_i, "Load the float zero value to the `i` register", {}
    );
    register_instruction(
        OpCodes::op_float_load_one_r, op_float_load_one_r, "Load the float one value to the `r` register", {}
    );
    register_instruction(
        OpCodes::op_float_load_one_i, op_float_load_one_i, "Load the float one value to the `i` register", {}
    );
    register_instruction(
        OpCodes::op_float_load_r, op_float_load_r, "Load the float value to the `r` register", {Operand::Float}
    );
    register_instruction(
        OpCodes::op_float_load_i, op_float_load_i, "Load the float value to the `i` register", {Operand::Float}
    );
    register_instruction(
        OpCodes::op_float_absolute, op_float_absolute, "Get the absolute value of the `r` register", {}
    );
    register_instruction(OpCodes::op_float_negate, op_float_negate, "Negate the `r` register float value", {});
    register_instruction(OpCodes::op_float_add, op_float_add, "Add the `r` and `i` register float values", {});
    register_instruction(
        OpCodes::op_float_subtract, op_float_subtract, "Subtract the `r` and `i` register float values", {}
    );
    register_instruction(
        OpCodes::op_float_multiply, op_float_multiply, "Multiply the `r` and `i` register float values", {}
    );
    register_instruction(OpCodes::op_float_divide, op_float_divide, "Divide the `r` and `i` register float values", {});
    register_instruction(OpCodes::op_float_modulo, op_float_modulo, "Modulo the `r` and `i` register float values", {});
    register_instruction(OpCodes::op_float_sin, op_float_sin, "Get the sine value of the `r` register", {});
    register_instruction(OpCodes::op_float_cosine, op_float_cosine, "Get the cosine value of the `r` register", {});
    register_instruction(OpCodes::op_float_tangent, op_float_tangent, "Get the tangent value of the `r` register", {});
    register_instruction(
        OpCodes::op_float_equal, op_float_equal, "Test if the `r` and `i` register float values are equal", {}
    );
    register_instruction(
        OpCodes::op_float_not_equal, op_float_not_equal, "Test if the `r` and `i` register float values are not equal",
        {}
    );
    register_instruction(
        OpCodes::op_float_less_than, op_float_less_than,
        "Test if the `r` register float value is less than the `i` register float value", {}
    );
    register_instruction(
        OpCodes::op_float_less_than_or_equal, op_float_less_than_or_equal, "op_float_less_than_or_equal", {}
    );
    register_instruction(
        OpCodes::op_float_greater_than, op_float_greater_than,
        "Test if the `r` register float value is greater than the `i` register float value", {}
    );
    register_instruction(
        OpCodes::op_float_greater_than_or_equal, op_float_greater_than_or_equal,
        "Test if the `r` register float value is greater than or equal to the `i` register float value", {}
    );

    // String operators
    register_instruction(
        OpCodes::op_string_init_r, op_string_init_r, "Init an empty (null) string in the `r` register", {}
    );
    register_instruction(
        OpCodes::op_string_init_i, op_string_init_i, "Init an empty (null) string in the `i` register", {}
    );
    register_instruction(OpCodes::op_string_free_r, op_string_free_r, "Free the string in the `r` register", {});
    register_instruction(OpCodes::op_string_free_i, op_string_free_i, "Free the string in the `i` register", {});
    register_instruction(
        OpCodes::op_string_free_variable, op_string_free_variable, "Free the string script variable", {Operand::String}
    );
    register_instruction(
        OpCodes::op_string_free_stack, op_string_free_stack, "Free the string parameter variable", {Operand::Id}
    );
    register_instruction(
        OpCodes::op_string_push_r, op_string_push_r, "Push the `r` register string value to the stack", {}
    );
    register_instruction(
        OpCodes::op_string_push_i, op_string_push_i, "Push the `i` register string value to the stack", {}
    );
    register_instruction(OpCodes::op_string_pop_r, op_string_pop_r, "Pop the string value to the `r` register", {});
    register_instruction(OpCodes::op_string_pop_i, op_string_pop_i, "Pop the string value to the `i` register", {});
    register_instruction(
        OpCodes::op_string_load_r, op_string_load_r, "Load the string value to the `r` register", {Operand::String}
    );
    register_instruction(
        OpCodes::op_string_load_i, op_string_load_i, "Load the string value to the `i` register", {Operand::String}
    );
    register_instruction(
        OpCodes::op_string_equal, op_string_equal, "Test if the `r` and `i` register string values are equal", {}
    );
    register_instruction(
        OpCodes::op_string_not_equal, op_string_not_equal,
        "Test if the `r` and `i` register string values are not equal", {}
    );

    // Format string operators
    register_instruction(
        OpCodes::op_format_string_execute, op_format_string_execute, "Execute the format string in the `r` register",
        {Operand::FormatString}
    );

    // ID operators
    register_instruction(OpCodes::op_id_push, op_id_push, "Push the `id` value to the stack", {Operand::Id});
    register_instruction(OpCodes::op_id_load_r, op_id_load_r, "Load the `id` value to the `r` register", {});
    register_instruction(OpCodes::op_id_load_i, op_id_load_i, "Load the `id` value to the `i` register", {});

    // Debug
    register_instruction(
        OpCodes::op_assert, op_assert, "Assert the boolean value in the `r` register value is true", {}
    );
}

} // namespace nomad