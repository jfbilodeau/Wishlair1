//
// Created by jfbil on 2023-06-05.
//
#include "doctest.h"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/script/Runtime.hpp"

#include "nomad/ConsoleSink.hpp"

#include "nomad/Nomad.hpp"

TEST_CASE("Compiler") {
    SUBCASE("empty source") {
        auto source = "";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));
    }

    SUBCASE("assert") {
        auto source = "assert 1 == 1";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        CHECK((script->get_op_code(0) == runtime.get_instruction_id("_assert")));
        auto assertion_message_id = script->get_id(1);
        auto assertion_message = runtime.get_string(assertion_message_id);
        CHECK((script->get_op_code(1) == assertion_message_id));
        CHECK((script->get_op_code(2) == runtime.get_operator_id("op_equal")));
        CHECK((script->get_op_code(3) == runtime.get_operator_id("op_load_one")));
        CHECK((script->get_op_code(4) == runtime.get_operator_id("op_load_one")));
        CHECK((script->get_op_code(5) == runtime.get_instruction_id("_return")));
        CHECK((script->get_op_code(6) == runtime.get_operator_id("op_load_zero")));
    }

    SUBCASE("const") {
        auto source = "const a = 1\nassert a == 1";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        CHECK((script->get_op_code(0) == runtime.get_instruction_id("_set_constant")));
        CHECK((script->get_op_code(1) == runtime.get_constant_id("a")));
        CHECK((script->get_op_code(2) == runtime.get_operator_id("op_load_one")));
        CHECK((script->get_op_code(3) == runtime.get_instruction_id("_assert")));
        CHECK((script->get_op_code(4) == 0));  // assertion message
        CHECK((script->get_op_code(5) == runtime.get_operator_id("op_equal")));
        CHECK((script->get_op_code(6) == runtime.get_operator_id("op_load_constant")));
        CHECK((script->get_op_code(7) == runtime.get_constant_id("a")));
        CHECK((script->get_op_code(8) == runtime.get_operator_id("op_load_one")));
        CHECK((script->get_op_code(9) == runtime.get_instruction_id("_return")));
        CHECK((script->get_op_code(10) == runtime.get_operator_id("op_load_zero")));
    }
}
