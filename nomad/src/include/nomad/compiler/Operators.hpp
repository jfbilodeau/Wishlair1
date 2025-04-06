//
// Created by jfbil on 2023-06-05.
//

#pragma once

#include "nomad/nomad.hpp"

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

UnaryOperator getUnaryOperator(const NomadString& symbol);
NomadString toString(UnaryOperator op);

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

BinaryOperator getBinaryOperator(const NomadString& symbol);
NomadString toString(BinaryOperator op);

// Folding functions

// Unary operator implementations

// Boolean unary operators
void foldBooleanUnaryBang(const ScriptValue& value, ScriptValue& result);

// Integer unary operators
void foldIntegerUnaryPlus(const ScriptValue& value, ScriptValue& result);
void foldIntegerUnaryMinus(const ScriptValue& value, ScriptValue& result);

// Float unary operators
void foldFloatUnaryPlus(const ScriptValue& value, ScriptValue& result);
void foldFloatUnaryMinus(const ScriptValue& value, ScriptValue& result);
void foldFloatUnarySin(const ScriptValue& value, ScriptValue& result);
void foldFloatUnaryCos(const ScriptValue& value, ScriptValue& result);
void foldFloatUnaryTan(const ScriptValue& value, ScriptValue& result);

// Binary operator implementations

// Boolean binary operators
void foldBooleanBinaryAndAnd(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldBooleanBinaryPipePipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);

// Integer binary operators
void foldIntegerBinaryPlus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryMinus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryStar(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinarySlash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryPercent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryCaret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerAnd(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerPipe(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryEqualEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryBangEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryLessThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryLessThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryGreaterThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldIntegerBinaryGreaterThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);

// Float binary operators
void foldFloatBinaryPlus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryMinus(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryStar(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinarySlash(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryPercent(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryCaret(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryEqualEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryBangEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryLessThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryLessThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryGreaterThan(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);
void foldFloatBinaryGreaterThanEqual(const ScriptValue& lhs, const ScriptValue& rhs, ScriptValue& result);

} // nomad

