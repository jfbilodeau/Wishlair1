#include "doctest.h"

#include "nomad/Logger.hpp"

#include "nomad/compiler/Compiler.hpp"
#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/script/Runtime.hpp"

TEST_CASE("FormatString") {
    SUBCASE("Empty") {
        nomad::Runtime runtime;
        nomad::Compiler compiler(&runtime);
        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, "string_assert $\"\" \"\"");

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

         auto interpreter = runtime.create_interpreter(script_id);
        interpreter->run();
    }

    SUBCASE("variable") {
        auto source =
            "a = 1\n"
            "b = 2.5\n"
            "string_assert $\"{a} {b}\" \"1 2.5\"\n";

        nomad::Runtime runtime;
        nomad::Compiler compiler(&runtime);

        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);
        interpreter->run();
    }

    SUBCASE("string operator") {
        auto source =
            "a = \"abc\"\n"
            "string_assert $\"{string a}\" \"abc\"\n";

        nomad::Runtime runtime;
        nomad::Compiler compiler(&runtime);
        auto script_id = runtime.register_script("test");
        auto script = runtime.get_script(script_id);

        compiler.compile_script(script, source);

        CHECK((script_id != nomad::NOMAD_INVALID_ID));

        auto interpreter = runtime.create_interpreter(script_id);

        interpreter->run();
    }
}