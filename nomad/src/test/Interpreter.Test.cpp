//
// Created by jfbil on 2023-06-05.
//
#include "doctest.h"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

#include "nomad/ConsoleSink.hpp"

#include "nomad/Nomad.hpp"

TEST_CASE("Interpreter") {
    SUBCASE("execute empty source") {
        auto source = "";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("execute assert success") {
        auto source = "assert true == true";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("execute assert failure") {
        auto source = "assert false == true";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        CHECK_THROWS(interpreter->run());
    }

    SUBCASE("const") {
        auto source = "const a = 10\nassert a == 10";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
        CHECK((runtime.get_constant_value(1) == 10));
    }

    SUBCASE("set script variable") {
        auto source = "a = 10\nassert a == 10";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
        CHECK((interpreter->get_variable_value(0) == 10));
    }

    SUBCASE("const script") {
        auto source =
            "assert PI == 3.1415926535897932385\n"
            "const TEST = 123\n"
            "assert TEST == 123";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("operators test") {
        auto source =
            "# Operator test\n"
            "\n"
            "# Boolean operators\n"
            "assert 1.0\n"
            "assert 1 == true\n"
            "assert 0.0 == false\n"
            "assert true == 1\n"
            "assert false == 0\n"
            "\n"
            "assert true\n"
            "assert false == false\n"
            "assert not false\n"
            "assert (true && true)\n"
            "assert (true && false) == false\n"
            "assert (false && true) == false\n"
            "assert (false && false) == false\n"
            "assert (false || true)\n"
            "assert (true || false)\n"
            "assert (true || true)\n"
            "assert (false || false) == false\n"
            "\n"
            "# Relational operators\n"
            "assert 1 == 1\n"
            "assert 0 != 1\n"
            "assert 1 > 0\n"
            "assert 0 < 1\n"
            "assert 1 >= 1\n"
            "assert 1 <= 1\n"
            "assert 2 >= 1\n"
            "assert 0 <= 1\n"
            "\n"
            "# Arithmetic operators\n"
            "assert 1 + 1 == 2\n"
            "assert 1 - 1 == 0\n"
            "assert 2 * 3 == 6\n"
            "assert 6 / 2 == 3\n"
            "assert 5 % 2 == 1\n"
            "\n"
            "# Precedence of operation\n"
            "assert 1 + 2 * 3 == 7\n"
            "assert (1 + 2) * 3 == 9\n"
            "\n"
            "# 'Big' numbers that overflows 16 bits\n"
            "assert 100000 + 200000 == 300000\n"
            "\n"
            "# Negative numbers\n"
            "assert -1 + -2 == -3";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("variable test") {
        auto source =
            "a = 2\n"
            "test.a = 3\n";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);
        runtime.register_variable_context("test", "test.", std::make_unique<nomad::SimpleVariableContext>());

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));

        CHECK_EQ(interpreter->get_variable_value("a"), 2);
        CHECK_EQ(interpreter->get_variable_value("test.a"), 3);
        CHECK((interpreter->get_variable_value("b") == 0.0)); // 'b' does not exist -- should return 0.0
    }

    SUBCASE("if statement test") {
        auto source =
            "m_x = 10\n"
            "\n"
            "if m_x == 10\n"
            "    assert true\n"
            "endIf\n"
            "\n"
            "if m_x != 10\n"
            "    assert false\n"
            "endIf\n"
            "\n"
            "if m_x == 10\n"
            "    assert true\n"
            "else\n"
            "    assert false\n"
            "endIf\n"
            "\n"
            "if m_x != 10\n"
            "    assert false\n"
            "else\n"
            "    assert true\n"
            "endIf\n"
            "\n"
            "if m_x == 10\n"
            "    assert true\n"
            "elseIf m_x == 11\n"
            "    assert false\n"
            "elseIf m_x == 12\n"
            "    assert false\n"
            "endIf\n"
            "\n"
            "if m_x == 9\n"
            "    assert false\n"
            "elseIf m_x == 10\n"
            "    assert true\n"
            "elseIf m_x == 11\n"
            "    assert false\n"
            "endIf\n"
            "\n"
            "if m_x == 8\n"
            "    assert false\n"
            "elseIf m_x == 9\n"
            "    assert false\n"
            "elseIf m_x == 10\n"
            "    assert true\n"
            "endIf\n"
            "\n"
            "if m_x == 7\n"
            "    assert false\n"
            "elseIf m_x == 8\n"
            "    assert false\n"
            "elseIf m_x == 9\n"
            "    assert false\n"
            "else   \n"
            "    assert true\n"
            "endIf\n"
            "\n"
            "if m_x == 10\n"
            "    assert true\n"
            "elseIf m_x == 11\n"
            "    assert false\n"
            "elseIf m_x == 12\n"
            "    assert false\n"
            "else   \n"
            "    assert false\n"
            "endIf\n"
            "\n"
            "# Embedded if statement\n"
            "if m_x == 10\n"
            "    if m_x == 10\n"
            "        assert true\n"
            "    else    \n"
            "        assert false\n"
            "    endIf\n"
            "endIf\n"
            "\n"
            "global.x = 0\n"
            "\n"
            "if true\n"
            "    global.x = 1\n"
            "else\n"
            "    global.x = 2\n"
            "endIf\n"
            "\n"
            "assert global.x == 1\n"
            "\n"
            "global.x = 0\n"
            "\n"
            "if false\n"
            "    global.x = 1\n"
            "else\n"
            "    global.x = 2\n"
            "endIf\n"
            "\n"
            "assert global.x == 2\n"
            "\n"
            "global.x = 0\n"
            "if 1 == 1\n"
            "    global.x = 1\n"
            "elseIf 1 == 2\n"
            "    global.x = 2\n"
            "else\n"
            "    global.x = 3\n"
            "endIf\n"
            "\n"
            "assert global.x == 1\n"
            "\n"
            "global.x = 0\n"
            "if 1 == 2\n"
            "    global.x = 1\n"
            "elseIf 1 == 1\n"
            "    global.x = 2\n"
            "else\n"
            "    global.x = 3\n"
            "endIf\n"
            "\n"
            "global.x = 0\n"
            "if 1 == 2\n"
            "    global.x = 1\n"
            "elseIf 1 == 3\n"
            "    global.x = 2\n"
            "else\n"
            "    global.x = 3\n"
            "endIf\n"
            "\n"
            "assert global.x == 3\n"
            "\n";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);
        runtime.register_variable_context("global", "global.", std::make_unique<nomad::SimpleVariableContext>());

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("return statement test") {
        auto source = "return 1";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        auto result = interpreter->run();

        CHECK((result == 1));
        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("fun test") {
        auto source =
                "global.a = 1\n"
                "\n"
                "fun add1\n"
                "   global.a = 2\n"
                "end\n"
                "\n"
                "add1\n"
                "\n"
                "assert global.a == 2\n"
                "\n"
                "fun add2\n"
                "   return 3\n"
                "end\n"
                "\n"
                "assert 3 == add2\n";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        auto result = interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }

    SUBCASE("fun parameters test") {
        auto source =
                "global.a = 1\n"
                "\n"
                "fun add1 value1 value2\n"
                "   global.a = value1 + value2\n"
                "end\n"
                "\n"
                "add1 1 2\n"
                "\n"
                "assert global.a == 3\n"
                "\n"
                "fun add2 value1 value2\n"
                "   return value1 + value2\n"
                "end\n"
                "\n"
                "assert 3 == add2 1 2\n";

        auto console_sink = std::make_shared<nomad::ConsoleSink>();
        auto logger = std::make_shared<nomad::Logger>(console_sink);

        nomad::Runtime runtime(logger);

        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        auto result = interpreter->run();

        CHECK((interpreter->is_running() == false));
        CHECK((interpreter->has_yielded() == false));
    }
}

