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

UnaryOperator getUnaryOperator(const NomadString& symbol) {
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

NomadString toString(UnaryOperator op) {
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

BinaryOperator getBinaryOperator(const NomadString& symbol) {
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

NomadString toString(BinaryOperator op) {
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

void foldBooleanUnaryBang(const ScriptValue& value, ScriptValue& result) {
    auto booleanValue = value.getBooleanValue();

    auto booleanResult = booleanNot(booleanValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerUnaryPlus(const ScriptValue& value, ScriptValue& result) {
    auto integerValue = value.getIntegerValue();

    auto integerResult = integerAbsolute(integerValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerUnaryMinus(const ScriptValue& value, ScriptValue& result) {
    auto integerValue = value.getIntegerValue();

    auto integerResult = integerNegate(integerValue);

    result.setIntegerValue(integerResult);
}

void foldFloatUnaryPlus(const ScriptValue& value, ScriptValue& result) {
    auto floatValue = value.getFloatValue();

    auto floatResult = floatAbsolute(floatValue);

    result.setFloatValue(floatResult);
}

void foldFloatUnaryMinus(const ScriptValue& value, ScriptValue& result) {
    auto floatValue = value.getFloatValue();

    auto floatResult = floatNegate(floatValue);

    result.setFloatValue(floatResult);
}

void foldFloatUnarySin(const ScriptValue& value, ScriptValue& result) {
    auto floatValue = value.getFloatValue();

    auto floatResult = floatSin(floatValue);

    result.setFloatValue(floatResult);
}

void foldFloatUnaryCos(const ScriptValue& value, ScriptValue& result) {
    auto floatValue = value.getFloatValue();

    auto floatResult = floatCosine(floatValue);

    result.setFloatValue(floatResult);
}

void foldFloatUnaryTan(const ScriptValue& value, ScriptValue& result) {
    auto floatValue = value.getFloatValue();

    auto floatResult = floatTangent(floatValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinaryPlus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatAdd(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinaryMinus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatSubtract(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinaryStar(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatMultiply(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinarySlash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatDivide(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldBooleanBinaryAndAnd(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getBooleanValue();
    auto rhsValue = rhs.getBooleanValue();

    auto booleanResult = booleanAnd(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldBooleanBinaryPipePipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getBooleanValue();
    auto rhsValue = rhs.getBooleanValue();

    auto booleanResult = booleanOr(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryPlus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerAdd(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinaryMinus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerSubtract(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinaryStar(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerMultiply(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinarySlash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerDivide(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinaryPercent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerModulo(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerAnd(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerAnd(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerPipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerOr(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinaryCaret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto integerResult = integerXor(lhsValue, rhsValue);

    result.setIntegerValue(integerResult);
}

void foldIntegerBinaryEqualEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryBangEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerNotEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryLessThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerLessThan(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryLessThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerLessThanOrEqual(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryGreaterThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerGreaterThan(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldIntegerBinaryGreaterThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getIntegerValue();
    auto rhsValue = rhs.getIntegerValue();

    auto booleanResult = integerGreaterThanOrEqual(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryPercent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatModulo(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinaryCaret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto floatResult = floatPower(lhsValue, rhsValue);

    result.setFloatValue(floatResult);
}

void foldFloatBinaryEqualEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryBangEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatNotEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryLessThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatLessThan(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryLessThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatLessThanOrEqual(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryGreaterThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatGreaterThan(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldFloatBinaryGreaterThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getFloatValue();
    auto rhsValue = rhs.getFloatValue();

    auto booleanResult = floatGreaterThanOrEqual(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldStringBinaryPlus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getStringValue();
    auto rhsValue = rhs.getStringValue();

    auto stringResult = stringConcatenate(lhsValue, rhsValue);

    result.setStringValue(stringResult);
}

void foldStringBinaryEqualEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getStringValue();
    auto rhsValue = rhs.getStringValue();

    auto booleanResult = stringEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

void foldStringBinaryBangEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result) {
    auto lhsValue = lhs.getStringValue();
    auto rhsValue = rhs.getStringValue();

    auto booleanResult = stringNotEqualTo(lhsValue, rhsValue);

    result.setBooleanValue(booleanResult);
}

} // nomad