//
// Created by jfbil on 2024-12-07.
//

#pragma once

#include "nomad/compiler/Compiler.hpp"

namespace nomad {

std::unique_ptr<StatementNode> parseSelectStatement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // nomad

