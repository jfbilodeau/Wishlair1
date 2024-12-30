//
// Created by jfbil on 2024-12-07.
//

#ifndef NOMAD_GAMESTATEMENTPARSERS_HPP
#define NOMAD_GAMESTATEMENTPARSERS_HPP

#include "nomad/compiler/Compiler.hpp"

namespace nomad {

std::unique_ptr<StatementNode> parse_select_statement(Compiler* compiler, Script* script, Tokenizer* tokens);

} // nomad

#endif //NOMAD_GAMESTATEMENTPARSERS_HPP
