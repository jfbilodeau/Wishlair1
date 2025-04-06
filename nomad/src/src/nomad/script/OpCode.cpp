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
    interpreter->stop(interpreter->getResult());
}

void op_yield(Interpreter* interpreter) {
    interpreter->yield(interpreter->getResult());
}

void op_return(Interpreter* interpreter) {
    interpreter->returnScript(0);
}

void op_return_n(Interpreter* interpreter) {
    auto variable_count = interpreter->nextIndex();

    interpreter->returnScript(variable_count);
}

void op_jump(Interpreter* interpreter) {
    auto jump_index = interpreter->nextIndex();

    interpreter->jump(jump_index);
}

void op_jump_if(Interpreter* interpreter) {
    auto jump_index = interpreter->nextIndex();
    auto condition = interpreter->getBooleanResult();

    if (condition) {
        interpreter->jump(jump_index);
    }
}

void op_jump_if_false(Interpreter* interpreter) {
    auto jump_index = interpreter->nextIndex();
    auto condition = interpreter->getBooleanResult();

    if (!condition) {
        interpreter->jump(jump_index);
    }
}

void op_push_r(Interpreter* interpreter) {
    interpreter->pushResult();
}

void op_push_i(Interpreter* interpreter) {
    interpreter->pushIntermediate();
}

void op_push_n(Interpreter* interpreter) {
    NomadInteger push_count = interpreter->nextInteger();

    interpreter->pushN(push_count);
}

void op_pop_r(Interpreter* interpreter) {
    interpreter->popResult();
}

void op_pop_i(Interpreter* interpreter) {
    interpreter->popIntermediate();
}

void op_pop_n(Interpreter* interpreter) {
    NomadInteger pop_count = interpreter->nextInteger();

    interpreter->popN(pop_count);
}

void op_call_script(Interpreter* interpreter) {
    auto jump_index = interpreter->nextIndex();

    interpreter->callScript(jump_index);
}

void op_call_command(Interpreter* interpreter) {
    auto command_id = interpreter->nextId();

    interpreter->callCommand(command_id);
}

void op_parameter_load_r(Interpreter* interpreter) {
    auto parameter_id = interpreter->nextId();

    auto& value = interpreter->getParameter(parameter_id);

    interpreter->setResult(value);
}

void op_string_parameter_load_r(Interpreter* interpreter) {
    auto parameter_id = interpreter->nextId();

    auto value = interpreter->getStringParameter(parameter_id);

    interpreter->setStringResult(value);
}

void op_script_variable_set(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    interpreter->setScriptVariableValue(variable_id, interpreter->getResult());
}

void op_script_variable_string_set(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    interpreter->setStringVariableValue(variable_id, interpreter->getResult().getStringValue());
}

void op_script_variable_get(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    ScriptValue value;
    interpreter->getScriptVariableValue(variable_id, value);

    interpreter->setResult(value);
}

void op_script_variable_string_get(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    auto value = interpreter->getScriptStringVariableValue(variable_id);

    interpreter->setStringResult(value);
}

void op_dynamic_variable_set(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    interpreter->getRuntime()->setDynamicVariable(interpreter, variable_id, interpreter->getResult());
}

void op_dynamic_variable_string_set(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    interpreter->getRuntime()->setStringDynamicVariable(interpreter, variable_id, interpreter->getResult());
}

void op_dynamic_variable_get(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    ScriptValue value;

    interpreter->getRuntime()->getDynamicVariableValue(interpreter, variable_id, value);

    interpreter->setResult(value);
}

void op_dynamic_variable_string_get(Interpreter* interpreter) {
    auto variable_id = interpreter->nextId();

    NomadString value;

    interpreter->getRuntime()->getStringDynamicVariableValue(interpreter, variable_id, value);

    interpreter->setStringResult(value);
}

void op_context_variable_set(Interpreter* interpreter) {
    auto context_id = interpreter->nextId();
    auto variable_id = interpreter->nextId();

    interpreter->getRuntime()->setContextVariableValue(context_id, variable_id, interpreter->getResult());
}

void op_context_variable_string_set(Interpreter* interpreter) {
    auto context_id = interpreter->nextId();
    auto variable_id = interpreter->nextId();

    interpreter->getRuntime()->setStringContextVariableValue(context_id, variable_id, interpreter->getResult().getStringValue());
}

void op_context_variable_get(Interpreter* interpreter) {
    auto context_id = interpreter->nextId();
    auto variable_id = interpreter->nextId();

    ScriptValue value;

    interpreter->getRuntime()->getContextVariableValue(context_id, variable_id, value);

    interpreter->setResult(value);
}

void op_context_variable_string_get(Interpreter* interpreter) {
    auto context_id = interpreter->nextId();
    auto variable_id = interpreter->nextId();

    NomadString value;

    interpreter->getRuntime()->get_string_context_variable_value(context_id, variable_id, value);

    interpreter->setStringResult(value);
}

void op_boolean_load_false_r(Interpreter* interpreter) {
    interpreter->setBooleanResult(NOMAD_FALSE);
}

void op_boolean_load_false_i(Interpreter* interpreter) {
    interpreter->setBooleanIntermediate(NOMAD_FALSE);
}

void op_boolean_load_true_r(Interpreter* interpreter) {
    interpreter->setBooleanResult(NOMAD_TRUE);
}

void op_boolean_load_true_i(Interpreter* interpreter) {
    interpreter->setBooleanIntermediate(NOMAD_TRUE);
}

void op_boolean_not(Interpreter* interpreter) {
    auto a = interpreter->getBooleanResult();

    auto result = booleanNot(a);

    interpreter->setBooleanResult(result);
}

void op_boolean_and(Interpreter* interpreter) {
    auto a = interpreter->getBooleanResult();
    auto b = interpreter->getBooleanIntermediate();

    auto result = booleanAnd(a, b);

    interpreter->setBooleanResult(result);
}

void op_boolean_or(Interpreter* interpreter) {
    auto a = interpreter->getBooleanResult();
    auto b = interpreter->getBooleanIntermediate();

    auto result = booleanOr(a, b);

    interpreter->setBooleanResult(result);
}

void op_boolean_equal(Interpreter* interpreter) {
    auto a = interpreter->getBooleanResult();
    auto b = interpreter->getBooleanIntermediate();

    auto result = booleanEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_boolean_not_equal(Interpreter* interpreter) {
    auto a = interpreter->getBooleanResult();
    auto b = interpreter->getBooleanIntermediate();

    auto result = booleanNotEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_load_zero_r(Interpreter* interpreter) {
    interpreter->setIntegerResult(0);
}

void op_integer_load_zero_i(Interpreter* interpreter) {
    interpreter->setIntegerIntermediate(0);
}

void op_integer_load_one_r(Interpreter* interpreter) {
    interpreter->setIntegerResult(1);
}

void op_integer_load_one_i(Interpreter* interpreter) {
    interpreter->setIntegerIntermediate(1);
}

void op_integer_load_r(Interpreter* interpreter) {
    auto value = interpreter->nextInteger();

    interpreter->setIntegerResult(value);
}

void op_integer_load_i(Interpreter* interpreter) {
    auto value = interpreter->nextInteger();

    interpreter->setIntegerIntermediate(value);
}

void op_integer_absolute(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();

    auto result = integerAbsolute(a);

    interpreter->setIntegerResult(result);
}

void op_integer_negate(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();

    auto result = integerNegate(a);

    interpreter->setIntegerResult(result);
}

void op_integer_add(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerAdd(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_subtract(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerSubtract(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_multiply(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerMultiply(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_divide(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerDivide(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_modulo(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerModulo(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_and(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerAnd(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_or(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerOr(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_xor(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerXor(a, b);

    interpreter->setIntegerResult(result);
}

void op_integer_equal(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_not_equal(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerNotEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_less_than(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerLessThan(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_less_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerLessThanOrEqual(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_greater_than(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerGreaterThan(a, b);

    interpreter->setBooleanResult(result);
}

void op_integer_greater_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->getIntegerResult();
    auto b = interpreter->getIntegerIntermediate();

    auto result = integerGreaterThanOrEqual(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_load_zero_r(Interpreter* interpreter) {
    interpreter->setFloatResult(0.0);
}

void op_float_load_zero_i(Interpreter* interpreter) {
    interpreter->setFloatIntermediate(0.0);
}

void op_float_load_one_r(Interpreter* interpreter) {
    interpreter->setFloatResult(1.0);
}

void op_float_load_one_i(Interpreter* interpreter) {
    interpreter->setFloatIntermediate(1.0);
}

void op_float_load_r(Interpreter* interpreter) {
    auto value = interpreter->nextFloat();

    interpreter->setFloatResult(value);
}

void op_float_load_i(Interpreter* interpreter) {
    auto value = interpreter->nextFloat();

    interpreter->setFloatIntermediate(value);
}

void op_float_absolute(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();

    auto result = floatAbsolute(a);

    interpreter->setFloatResult(result);
}

void op_float_negate(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();

    auto result = floatNegate(a);

    interpreter->setFloatResult(result);
}

void op_float_add(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatAdd(a, b);

    interpreter->setFloatResult(result);
}

void op_float_subtract(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatSubtract(a, b);

    interpreter->setFloatResult(result);
}

void op_float_multiply(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatMultiply(a, b);

    interpreter->setFloatResult(result);
}

void op_float_divide(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatDivide(a, b);

    interpreter->setFloatResult(result);
}

void op_float_modulo(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatModulo(a, b);

    interpreter->setFloatResult(result);
}

void op_float_sin(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();

    auto result = floatSin(a);

    interpreter->setFloatResult(result);
}

void op_float_cosine(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();

    auto result = floatCosine(a);

    interpreter->setFloatResult(result);
}

void op_float_tangent(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();

    auto result = floatTangent(a);

    interpreter->setFloatResult(result);
}

void op_float_equal(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_not_equal(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatNotEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_less_than(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatLessThan(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_less_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatLessThanOrEqual(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_greater_than(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatGreaterThan(a, b);

    interpreter->setBooleanResult(result);
}

void op_float_greater_than_or_equal(Interpreter* interpreter) {
    auto a = interpreter->getFloatResult();
    auto b = interpreter->getFloatIntermediate();

    auto result = floatGreaterThanOrEqual(a, b);

    interpreter->setBooleanResult(result);
}

void op_string_init(Interpreter* interpreter) {
    auto string_index = interpreter->nextIndex();

    interpreter->initStringVariable(string_index);
}

void op_string_init_r(Interpreter* interpreter) {
    interpreter->initStringResult();
}

void op_string_init_i(Interpreter* interpreter) {
    interpreter->initStringIntermediate();
}

void op_string_free_r(Interpreter* interpreter) {
    interpreter->freeStringResult();
}

void op_string_free_i(Interpreter* interpreter) {
    interpreter->freeStringIntermediate();
}

void op_string_free_variable(Interpreter* interpreter) {
    auto string_index = interpreter->nextIndex();

    interpreter->freeStringVariable(string_index);
}

void op_string_free_stack(Interpreter* interpreter) {
    auto index = interpreter->nextIndex();

    interpreter->freeStringStack(index);
}
void op_string_push_r(Interpreter* interpreter) {
    interpreter->pushStringResult();
}

void op_string_push_i(Interpreter* interpreter) {
    interpreter->pushStringIntermediate();
}

void op_string_pop_r(Interpreter* interpreter) {
    interpreter->popStringResult();
}

void op_string_pop_i(Interpreter* interpreter) {
    interpreter->popStringIntermediate();
}

void op_string_load_r(Interpreter* interpreter) {
    auto string_id = interpreter->nextId();

    auto& string = interpreter->getRuntime()->getString(string_id);

    interpreter->setStringResult(string);
}

void op_string_load_i(Interpreter* interpreter) {
    auto string_id = interpreter->nextId();

    auto string = interpreter->getRuntime()->getString(string_id);

    interpreter->setStringIntermediate(string);
}

void op_string_equal(Interpreter* interpreter) {
    auto a = interpreter->getResult().getStringValue();
    auto b = interpreter->getIntermediate().getStringValue();

    auto result = stringEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_string_not_equal(Interpreter* interpreter) {
    auto a = interpreter->getStringResult();
    auto b = interpreter->getStringIntermediate();

    auto result = stringNotEqualTo(a, b);

    interpreter->setBooleanResult(result);
}

void op_format_string_execute(Interpreter* interpreter) {
    auto format_string_id = interpreter->nextId();

    auto format_string = interpreter->getRuntime()->getFormatString(format_string_id);

    auto string = format_string->format(interpreter);

    interpreter->setStringResult(string);
}

void op_id_push(Interpreter* interpreter) {
    auto id = interpreter->nextId();

    interpreter->pushId(id);
}

void op_id_load_r(Interpreter* interpreter) {
    auto id = interpreter->nextId();

    interpreter->setIdResult(id);
}

void op_id_load_i(Interpreter* interpreter) {
    auto id = interpreter->nextId();

    interpreter->setIdIntermediate(id);
}

void op_assert(Interpreter* interpreter) {
    auto assert_result = interpreter->getBooleanResult();
    auto assert_message_id = interpreter->getIdIntermediate();

    // Bitwise comparison
    if (assert_result) {
        auto& assertion_message = interpreter->getRuntime()->getString(assert_message_id);

        log::error(assertion_message);

        interpreter->fault(assertion_message);
    }
}

} // namespace nomad
