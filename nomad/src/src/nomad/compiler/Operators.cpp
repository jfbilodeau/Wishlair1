//
// Created by jfbil on 2023-06-05.
//

#include "nomad/script/Interpreter.hpp"

#include "nomad/compiler/Operators.hpp"

#include "nomad/system/Boolean.hpp"
#include "nomad/system/Float.hpp"
#include "nomad/system/Integer.hpp"
#include "nomad/system/String.hpp"

namespace nomad {

UnaryOperator get_unary_operator(const NomadString& symbol) {
    if (symbol == "!") {
        return UnaryOperator::Bang;
    } else if (symbol == "+") {
        return UnaryOperator::Plus;
    } else if (symbol == "-") {
        return UnaryOperator::Minus;
    } else if (symbol == "sin") {
        return UnaryOperator::Sin;
    } else if (symbol == "cos") {
        return UnaryOperator::Cos;
    } else if (symbol == "tan") {
        return UnaryOperator::Tan;
    }

    return UnaryOperator::Unknown;
}

NomadString to_string(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::Bang:
            return "!";
        case UnaryOperator::Plus:
            return "+";
        case UnaryOperator::Minus:
            return "-";
        case UnaryOperator::Sin:
            return "sin";
        case UnaryOperator::Cos:
            return "cos";
        case UnaryOperator::Tan:
            return "tan";
        default:
            return "";
    }
}

BinaryOperator get_binary_operator(const NomadString& symbol) {
    if (symbol == "+") {
        return BinaryOperator::Plus;
    } else if (symbol == "-") {
        return BinaryOperator::Minus;
    } else if (symbol == "*") {
        return BinaryOperator::Star;
    } else if (symbol == "/") {
        return BinaryOperator::Slash;
    } else if (symbol == "^") {
        return BinaryOperator::Caret;
    } else if (symbol == "&") {
        return BinaryOperator::And;
    } else if (symbol == "%") {
        return BinaryOperator::Percent;
    } else if (symbol == "|") {
        return BinaryOperator::Pipe;
    } else if (symbol == "==") {
        return BinaryOperator::EqualEqual;
    } else if (symbol == "!=") {
        return BinaryOperator::BangEqual;
    } else if (symbol == "<") {
        return BinaryOperator::LessThan;
    } else if (symbol == "<=") {
        return BinaryOperator::LessThanEqual;
    } else if (symbol == ">") {
        return BinaryOperator::GreaterThan;
    } else if (symbol == ">=") {
        return BinaryOperator::GreaterThanEqual;
    } else if (symbol == "&&") {
        return BinaryOperator::AndAnd;
    } else if (symbol == "||") {
        return BinaryOperator::PipePipe;
    }

    return BinaryOperator::Unknown;
}

NomadString to_string(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Plus:
            return "+";
        case BinaryOperator::Minus:
            return "-";
        case BinaryOperator::Star:
            return "*";
        case BinaryOperator::Slash:
            return "/";
        case BinaryOperator::Percent:
            return "%";
        case BinaryOperator::Caret:
            return "^";
        case BinaryOperator::EqualEqual:
            return "==";
        case BinaryOperator::BangEqual:
            return "!=";
        case BinaryOperator::LessThan:
            return "<";
        case BinaryOperator::LessThanEqual:
            return "<=";
        case BinaryOperator::GreaterThan:
            return ">";
        case BinaryOperator::GreaterThanEqual:
            return ">=";
        case BinaryOperator::AndAnd:
            return "&&";
        case BinaryOperator::PipePipe:
            return "||";
        default:
            return "";
    }
}

void fold_boolean_unary_bang(const ScriptValue& value, ScriptValue& result) {
    auto boolean_value = value.get_boolean_value();

    auto boolean_result = boolean_not(boolean_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_unary_plus(const ScriptValue& value, ScriptValue& result) {
    auto integer_value = value.get_integer_value();

    auto integer_result = integer_absolute(integer_value);

    result.set_integer_value(integer_result);
}

void fold_integer_unary_minus(const ScriptValue& value, ScriptValue& result) {
    auto integer_value = value.get_integer_value();

    auto integer_result = integer_negate(integer_value);

    result.set_integer_value(integer_result);
}

void fold_float_unary_plus(const ScriptValue& value, ScriptValue& result) {
    auto float_value = value.get_float_value();

    auto float_result = float_absolute(float_value);

    result.set_float_value(float_result);
}

void fold_float_unary_minus(const ScriptValue& value, ScriptValue& result) {
    auto float_value = value.get_float_value();

    auto float_result = float_negate(float_value);

    result.set_float_value(float_result);
}

void fold_float_unary_sin(const ScriptValue& value, ScriptValue& result) {
    auto float_value = value.get_float_value();

    auto float_result = float_sin(float_value);

    result.set_float_value(float_result);
}

void fold_float_unary_cos(const ScriptValue& value, ScriptValue& result) {
    auto float_value = value.get_float_value();

    auto float_result = float_cosine(float_value);

    result.set_float_value(float_result);
}

void fold_float_unary_tan(const ScriptValue& value, ScriptValue& result) {
    auto float_value = value.get_float_value();

    auto float_result = float_tangent(float_value);

    result.set_float_value(float_result);
}

void fold_float_binary_plus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_add(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_float_binary_minus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_subtract(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_float_binary_star(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_multiply(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_float_binary_slash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_divide(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_boolean_binary_and_and(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_boolean_value();
    auto rhs_value = rhs.get_boolean_value();

    auto boolean_result = boolean_and(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_boolean_binary_pipe_pipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_boolean_value();
    auto rhs_value = rhs.get_boolean_value();

    auto boolean_result = boolean_or(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_plus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_add(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_minus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_subtract(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_star(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_multiply(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_slash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_divide(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_percent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_modulo(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_and(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_and(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_pipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_or(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_caret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto integer_result = integer_xor(lhs_value, rhs_value);

    result.set_integer_value(integer_result);
}

void fold_integer_binary_equal_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_bang_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_not_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_less_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_less_than(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_less_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_less_than_or_equal(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_greater_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_greater_than(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_integer_binary_greater_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_integer_value();
    auto rhs_value = rhs.get_integer_value();

    auto boolean_result = integer_greater_than_or_equal(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_percent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_modulo(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_float_binary_caret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto float_result = float_power(lhs_value, rhs_value);

    result.set_float_value(float_result);
}

void fold_float_binary_equal_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_bang_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_not_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_less_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_less_than(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_less_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_less_than_or_equal(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_greater_than(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_greater_than(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_float_binary_greater_than_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_float_value();
    auto rhs_value = rhs.get_float_value();

    auto boolean_result = float_greater_than_or_equal(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_string_binary_plus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_string_value();
    auto rhs_value = rhs.get_string_value();

    auto string_result = string_concatenate(lhs_value, rhs_value);

    result.set_string_value(string_result);
}

void fold_string_binary_equal_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_string_value();
    auto rhs_value = rhs.get_string_value();

    auto boolean_result = string_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}

void fold_string_binary_bang_equal(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhs_value = lhs.get_string_value();
    auto rhs_value = rhs.get_string_value();

    auto boolean_result = string_not_equal_to(lhs_value, rhs_value);

    result.set_boolean_value(boolean_result);
}


//
// struct UnaryOperatorInfo {
//     UnaryOperator op;
//     NomadString name;
//     UnaryOperatorFn* fn;
//     NomadString opcode;
// };
//
// const std::array<UnaryOperatorInfo, 6> unary_operators = {
//     UnaryOperatorInfo{ UnaryOperator::Not, "!", op_not, "op_not" },
//     UnaryOperatorInfo{ UnaryOperator::Plus, "+", op_plus, "op_unary_plus" },
//     UnaryOperatorInfo{ UnaryOperator::Minus, "-", op_minus, "op_unary_minus" },
//     UnaryOperatorInfo{ UnaryOperator::Sin, "sin", op_sin, "op_sin" },
//     UnaryOperatorInfo{ UnaryOperator::Cos, "cos", op_cos, "op_cos" },
//     UnaryOperatorInfo{ UnaryOperator::Tan, "tan", op_tan, "op_tan" },
// };
//
// UnaryOperator from_string(const NomadString& operator_name, UnaryOperator& op) {
//     for (auto& it : unary_operators) {
//         if (it.name == operator_name) {
//             op = it.op;
//             return op;
//         }
//     }
//
//     op = UnaryOperator::Unknown;
//     return op;
// }
//
// NomadString to_string(UnaryOperator op) {
//     for (auto& it : unary_operators) {
//         if (it.op == op) {
//             return it.name;
//         }
//     }
//
//     return "Unknown";
// }
//
// UnaryOperatorFn* get_unary_operator_fn(UnaryOperator op) {
//     for (auto& it : unary_operators) {
//         if (it.op == op) {
//             return it.fn;
//         }
//     }
//
//     return nullptr;
// }
//
// NomadString get_opcode_name(UnaryOperator op) {
//     for (auto& it : unary_operators) {
//         if (it.op == op) {
//             return it.opcode;
//         }
//     }
//
//     return "Unknown";
// }
//
// struct BinaryOperatorInfo {
//     BinaryOperator op;
//     NomadString name;
//     BinaryOperatorFn* fn;
//     NomadString opcode;
// };
//
// const std::array<BinaryOperatorInfo, 14> binary_operators = {
//     BinaryOperatorInfo{ BinaryOperator::Add, "+", op_add, "op_add" },
//     BinaryOperatorInfo{ BinaryOperator::Subtract, "-", op_sub, "op_sub" },
//     BinaryOperatorInfo{ BinaryOperator::Multiply, "*", op_mul, "op_mul" },
//     BinaryOperatorInfo{ BinaryOperator::Divide, "/", op_div, "op_div" },
//     BinaryOperatorInfo{ BinaryOperator::Modulo, "%", op_mod, "op_mod" },
//     BinaryOperatorInfo{ BinaryOperator::Power, "^", op_pow, "op_pow" },
//     BinaryOperatorInfo{ BinaryOperator::Equal, "==", op_equal, "op_equal" },
//     BinaryOperatorInfo{ BinaryOperator::NotEqual, "!=", op_not_equal, "op_not_equal" },
//     BinaryOperatorInfo{ BinaryOperator::LessThan, "<", op_less_than, "op_less_than" },
//     BinaryOperatorInfo{ BinaryOperator::LessThanOrEqual, "<=", op_less_than_or_equal, "op_less_than_or_equal" },
//     BinaryOperatorInfo{ BinaryOperator::GreaterThan, ">", op_greater_than, "op_greater_than" },
//     BinaryOperatorInfo{ BinaryOperator::GreaterThanOrEqual, ">=", op_greater_than_or_equal, "op_greater_than_or_equal" },
//     BinaryOperatorInfo{BinaryOperator::LogicalAnd, "&&", op_and, "op_and" },
//     BinaryOperatorInfo{BinaryOperator::LogicalOr, "||", op_or, "op_or" },
// };
//
// BinaryOperator from_string(const NomadString& operator_name, BinaryOperator& op) {
//     for (auto& it : binary_operators) {
//         if (it.name == operator_name) {
//             op = it.op;
//             return op;
//         }
//     }
//
//     op = BinaryOperator::Unknown;
//     return op;
// }
//
// NomadString to_string(BinaryOperator op) {
//     for (auto& it : binary_operators) {
//         if (it.op == op) {
//             return it.name;
//         }
//     }
//
//     return "Unknown";
// }
//
// BinaryOperatorFn* get_binary_operator_fn(BinaryOperator op) {
//     for (auto& it : binary_operators) {
//         if (it.op == op) {
//             return it.fn;
//         }
//     }
//
//     return nullptr;
// }
//
// NomadString get_opcode_name(BinaryOperator op) {
//     for (auto& it : binary_operators) {
//         if (it.op == op) {
//             return it.opcode;
//         }
//     }
//
//     return "Unknown";
// }
//
// NomadValue operator_load_zero(Interpreter* interpreter) {
//     return 0.0;
// }
//
// NomadValue operator_load_one(Interpreter* interpreter) {
//     return 1.0;
// }
//
// NomadValue operator_load_double(Interpreter* interpreter) {
//     return interpreter->next_double();
// }
//
// NomadValue operator_load_constant(Interpreter* interpreter) {
//     const auto constant_id = interpreter->next_id();
//
//     return interpreter->get_runtime()->get_constant_value(constant_id);
// }
//
// NomadValue operator_call_command(Interpreter* interpreter) {
//     const auto command_id = interpreter->next_id();
//
//     const auto command_fn = interpreter->get_runtime()->get_command_fn(command_id);
//
//     command_fn(interpreter);
//
//     return interpreter->get_result();
// }
//
// NomadValue operator_call_script(Interpreter* interpreter) {
//     const auto script_id = interpreter->next_id();
//
//     const auto script = interpreter->get_runtime()->get_script(script_id);
//
//     Interpreter script_interpreter(interpreter->get_runtime(), script);
//
//     return script_interpreter.run();
// }
//
// NomadValue operator_load_dynamic_variable(Interpreter* interpreter) {
//     const auto variable_id = interpreter->next_id();
//
//     return interpreter->get_runtime()->get_dynamic_variable_value(interpreter, variable_id);
// }
//
// NomadValue operator_load_variable(Interpreter* interpreter) {
//     const auto variable_context = interpreter->next_id();
//     const auto variable_id = interpreter->next_id();
//
//     return interpreter->get_runtime()->get_context_variable_value(variable_context, variable_id);
// }
//
// NomadValue operator_load_script_variable(Interpreter* interpreter) {
//     const auto variable_id = interpreter->next_id();
//
//     return interpreter->get_script_variable_value(variable_id);
// }
//
// NomadValue operator_load_string_id(Interpreter* interpreter) {
//     const auto string_id = interpreter->next_id();
//
//     return string_id;
// }
//
// NomadValue operator_not(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_not(value);
// }
//
// NomadValue operator_sin(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_sin(value);
// }
//
// NomadValue operator_cos(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_cos(value);
// }
//
// NomadValue operator_tan(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_tan(value);
// }
//
// NomadValue operator_unary_minus(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_minus(value);
// }
//
// NomadValue operator_unary_plus(Interpreter* interpreter) {
//     const auto value = interpreter->execute_expression();
//
//     return op_plus(value);
// }
//
// NomadValue operator_add(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_add(left, right);
// }
//
// NomadValue operator_sub(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_sub(left, right);
// }
//
// NomadValue operator_mul(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_mul(left, right);
// }
//
// NomadValue operator_div(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_div(left, right);
// }
//
// NomadValue operator_mod(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_mod(left, right);
// }
//
// NomadValue operator_pow(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_pow(left, right);
// }
//
// NomadValue operator_equal(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_equal(left, right);
// }
//
// NomadValue operator_not_equal(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_not_equal(left, right);
// }
//
// NomadValue operator_less_than(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_less_than(left, right);
// }
//
// NomadValue operator_less_than_or_equal(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_less_than_or_equal(left, right);
// }
//
// NomadValue operator_greater_than(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_greater_than(left, right);
// }
//
// NomadValue operator_greater_than_or_equal(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_greater_than_or_equal(left, right);
// }
//
// NomadValue operator_and(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_and(left, right);
// }
//
// NomadValue operator_or(Interpreter* interpreter) {
//     const auto left = interpreter->execute_expression();
//     const auto right = interpreter->execute_expression();
//
//     return op_or(left, right);
// }
//
} // nomad