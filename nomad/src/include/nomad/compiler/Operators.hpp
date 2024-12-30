//
// Created by jfbil on 2023-06-05.
//

#ifndef NOMAD_OPERATORS_HPP
#define NOMAD_OPERATORS_HPP

#include "nomad/Nomad.hpp"

#include "nomad/script/ScriptValue.hpp"

namespace nomad {

// Forward declarations
class Interpreter;

enum class UnaryOperator {
    Unknown = 1,
    Bang,
    Plus,
    Minus,
    Sin,
    Cos,
    Tan,
};

UnaryOperator get_unary_operator(const NomadString& symbol);
NomadString to_string(UnaryOperator op);
//NomadString get_opcode_name(UnaryOperator op);

enum class BinaryOperator {
    Unknown = 1,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Caret,
    And,
    Pipe,
    EqualEqual,
    BangEqual,
    LessThan,
    LessThanEqual,
    GreaterThan,
    GreaterThanEqual,
    AndAnd,
    PipePipe,
};

BinaryOperator get_binary_operator(const NomadString& symbol);
NomadString to_string(BinaryOperator op);

// Folding functions

// Unary operator implementations

// Boolean unary operators
void fold_boolean_unary_bang(const ScriptValue& value, ScriptValue& result);

// Integer unary operators
void fold_integer_unary_plus(const ScriptValue& value, ScriptValue& result);
void fold_integer_unary_minus(const ScriptValue& value, ScriptValue& result);

// Float unary operators
void fold_float_unary_plus(const ScriptValue& value, ScriptValue& result);
void fold_float_unary_minus(const ScriptValue& value, ScriptValue& result);
void fold_float_unary_sin(const ScriptValue& value, ScriptValue& result);
void fold_float_unary_cos(const ScriptValue& value, ScriptValue& result);
void fold_float_unary_tan(const ScriptValue& value, ScriptValue& result);

// Binary operator implementations

// Boolean binary operators
void fold_boolean_binary_and_and(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_boolean_binary_pipe_pipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);

// Integer binary operators
void fold_integer_binary_plus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_minus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_star(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_slash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_percent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_caret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_and(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_pipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_equal_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_bang_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_less_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_less_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_greater_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_integer_binary_greater_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);

// Float binary operators
void fold_float_binary_plus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_minus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_star(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_slash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_percent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_caret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_equal_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_bang_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_less_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_less_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_greater_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void fold_float_binary_greater_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);



//NomadString get_opcode_name(BinaryOperator op);

// Unary operator implementations
// inline NomadValue op_not(NomadValue value) { return value == 0 ? NOMAD_TRUE : NOMAD_FALSE; }
// inline NomadValue op_plus(NomadValue value) { return +value; }
// inline NomadValue op_minus(NomadValue value) { return -value; }
// inline NomadValue op_sin(NomadValue value) { return std::sin(value); }
// inline NomadValue op_cos(NomadValue value) { return std::cos(value); }
// inline NomadValue op_tan(NomadValue value) { return std::tan(value);}
//
// // Binary operator implementations
// inline NomadValue op_add(NomadValue lhs, NomadValue rhs) { return lhs + rhs; }
// inline NomadValue op_sub(NomadValue lhs, NomadValue rhs) { return lhs - rhs; }
// inline NomadValue op_mul(NomadValue lhs, NomadValue rhs) { return lhs * rhs; }
// inline NomadValue op_div(NomadValue lhs, NomadValue rhs) { return lhs / rhs; }
// inline NomadValue op_mod(NomadValue lhs, NomadValue rhs) { return std::fmod(lhs, rhs); }
// inline NomadValue op_pow(NomadValue lhs, NomadValue rhs) { return std::pow(lhs, rhs); }
// inline NomadValue op_equal(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs == rhs); }
// inline NomadValue op_not_equal(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs != rhs); }
// inline NomadValue op_less_than(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs < rhs); }
// inline NomadValue op_less_than_or_equal(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs <= rhs); }
// inline NomadValue op_greater_than(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs > rhs); }
// inline NomadValue op_greater_than_or_equal(NomadValue lhs, NomadValue rhs) { return NomadValue(lhs >= rhs); }
// inline NomadValue op_and(NomadValue lhs, NomadValue rhs) { return NomadValue(bool(lhs) && bool(rhs)); }
// inline NomadValue op_or(NomadValue lhs, NomadValue rhs) { return NomadValue(bool(lhs) || bool(rhs)); }
//
// // Interpreter implementations
// NomadValue operator_load_zero(Interpreter* interpreter);
// NomadValue operator_load_one(Interpreter* interpreter);
// NomadValue operator_load_double(Interpreter* interpreter);
// NomadValue operator_load_constant(Interpreter* interpreter);
// NomadValue operator_call_command(Interpreter* interpreter);
// NomadValue operator_call_script(Interpreter* interpreter);
// NomadValue operator_load_dynamic_variable(Interpreter* interpreter);
// NomadValue operator_load_variable(Interpreter* interpreter);
// NomadValue operator_load_script_variable(Interpreter* interpreter);
// NomadValue operator_load_string_id(Interpreter* interpreter);
// NomadValue operator_not(Interpreter* interpreter);
// NomadValue operator_sin(Interpreter* interpreter);
// NomadValue operator_cos(Interpreter* interpreter);
// NomadValue operator_tan(Interpreter* interpreter);
// NomadValue operator_unary_minus(Interpreter* interpreter);
// NomadValue operator_unary_plus(Interpreter* interpreter);
// NomadValue operator_add(Interpreter* interpreter);
// NomadValue operator_sub(Interpreter* interpreter);
// NomadValue operator_mul(Interpreter* interpreter);
// NomadValue operator_div(Interpreter* interpreter);
// NomadValue operator_mod(Interpreter* interpreter);
// NomadValue operator_equal(Interpreter* interpreter);
// NomadValue operator_not_equal(Interpreter* interpreter);
// NomadValue operator_less_than(Interpreter* interpreter);
// NomadValue operator_less_than_or_equal(Interpreter* interpreter);
// NomadValue operator_greater_than(Interpreter* interpreter);
// NomadValue operator_greater_than_or_equal(Interpreter* interpreter);
// NomadValue operator_and(Interpreter* interpreter);
// NomadValue operator_or(Interpreter* interpreter);

} // nomad

#endif //NOMAD_OPERATORS_HPP
