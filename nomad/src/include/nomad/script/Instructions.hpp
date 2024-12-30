//
// Created by jfbil on 2023-06-04.
//

#ifndef CPP_INSTRUCTIONS_HPP
#define CPP_INSTRUCTIONS_HPP

namespace nomad {

class Interpreter;

void instruction_nop(Interpreter * interpreter);
void instruction_return(Interpreter * interpreter);
void instruction_return_zero(Interpreter * interpreter);
void instruction_yield(Interpreter * interpreter);
void instruction_call_command(Interpreter* interpreter);
//void instruction_call_fun(Interpreter * interpreter);
void instruction_call_script(Interpreter * interpreter);
void instruction_jump(Interpreter * interpreter);
void instruction_if(Interpreter * interpreter);
void instruction_set_constant(Interpreter * interpreter);
void instruction_set_dynamic_variable(Interpreter * interpreter);
void instruction_set_context_variable(Interpreter * interpreter);
void instruction_set_script_variable(Interpreter * interpreter);
void instruction_assert(Interpreter * interpreter);
void instruction_string_assert(Interpreter * interpreter);

} // nomad

#endif //CPP_INSTRUCTIONS_HPP
