//
// Created by jfbil on 2023-06-04.
//

#pragma once

namespace nomad {

class Interpreter;

void instructionNop(Interpreter * interpreter);
void instructionReturn(Interpreter * interpreter);
void instructionReturnZero(Interpreter * interpreter);
void instructionYield(Interpreter * interpreter);
void instructionCallCommand(Interpreter* interpreter);
void instructionCallScript(Interpreter * interpreter);
void instructionJump(Interpreter * interpreter);
void instructionIf(Interpreter * interpreter);
void instructionSetConstant(Interpreter * interpreter);
void instructionSetDynamicVariable(Interpreter * interpreter);
void instructionSetContextVariable(Interpreter * interpreter);
void instructionSetScriptVariable(Interpreter * interpreter);
void instructionAssert(Interpreter * interpreter);
void instructionStringAssert(Interpreter * interpreter);

} // nomad

