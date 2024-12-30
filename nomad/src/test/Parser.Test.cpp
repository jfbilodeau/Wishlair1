#include "doctest.h"

#include "nomad/compiler/Tokenizer.hpp"

TEST_CASE("Parser") {
    SUBCASE("Empty") {
        auto parser = nomad::Parser("");

        CHECK(parser.end_of_file());
        CHECK(parser.end_of_tokens());
    }

    SUBCASE("Simple set of statements") {
        auto parser = nomad::Parser("a b 12 c");

        CHECK(!parser.end_of_file());
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "12"));
        CHECK((parser.next_token() == "c"));
        CHECK(parser.end_of_tokens());
    }

    SUBCASE("Parse multiple lines") {
        auto source = "a b\n12\n\nc d";

        auto parser = nomad::Parser(source);

        CHECK((parser.get_line_count() == 4));
        CHECK((parser.get_token_count() == 2));
        CHECK((parser.next_line()));
        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_line()));
        CHECK((parser.get_token_count() == 0));
        CHECK((parser.next_line()));
        CHECK((parser.get_token_count() == 2));
        CHECK((!parser.next_line()));
    }

    SUBCASE("Parse quoted strings") {
        auto source = "a \"middle\" d\n\"start\" b c\na b \"end\"\n\"quote\"";

        auto parser = nomad::Parser(source);

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "\"middle\""));
        CHECK((parser.next_token() == "d"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "\"start\""));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "c"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "\"end\""));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_token() == "\"quote\""));
        CHECK((parser.end_of_tokens()));
    }

    SUBCASE("Parse format strings") {
        auto source = "a $\"middle\" d\n$\"start\" b c\na b $\"end\"\n$\"quote\"";

        auto parser = nomad::Parser(source);

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "$\"middle\""));
        CHECK((parser.next_token() == "d"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "$\"start\""));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "c"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 3));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "$\"end\""));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_token() == "$\"quote\""));
        CHECK((parser.end_of_tokens()));
    }

    SUBCASE("Parse comments") {
        auto source = "a # comment\nb c # comment\n# comment\nd";

        auto parser = nomad::Parser(source);

        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 2));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "c"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 0));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_token() == "d"));
        CHECK((parser.end_of_tokens()));
    }

    SUBCASE("Parse comments with quotes") {
        auto source = "a \"# comment\"\nb c # comment\n# comment\nd";

        auto parser = nomad::Parser(source);

        CHECK((parser.get_token_count() == 2));
        CHECK((parser.next_token() == "a"));
        CHECK((parser.next_token() == "\"# comment\""));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 2));
        CHECK((parser.next_token() == "b"));
        CHECK((parser.next_token() == "c"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 0));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        CHECK((parser.get_token_count() == 1));
        CHECK((parser.next_token() == "d"));
        CHECK((parser.end_of_tokens()));
    }

    SUBCASE("Parse unterminated string literal") {
        auto source = "valid\na \"unterminated\nb c";

        auto parser = nomad::Parser(source);

        CHECK((!parser.get_error()));
        CHECK((parser.get_token_count() == 1));
        CHECK((parser.token_is("valid")));
        CHECK((parser.next_token() == "valid"));
        CHECK((parser.end_of_tokens()));
        CHECK((!parser.next_line()));

        CHECK((parser.get_error()));
    }

    SUBCASE("Parse parenthese") {
        auto source = "(1 + 2)\n1 + (1 + 2)\n1 + (2 + 3) + 4\n1 + (2 + 3)\n((1 + 2) + (1 + 3))";

        auto parser = nomad::Parser(source);

        // (1 + 2)
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "2"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        // 1 + (1 + 2)
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "2"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        // 1 + (2 + 3) + 4
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "2"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "3"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "4"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        // 1 + (2 + 3)
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "2"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "3"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.end_of_tokens()));
        CHECK((parser.next_line()));

        // ((1 + 2) + (1 + 3))
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "2"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "("));
        CHECK((parser.next_token() == "1"));
        CHECK((parser.next_token() == "+"));
        CHECK((parser.next_token() == "3"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.next_token() == ")"));
        CHECK((parser.end_of_tokens()));
        CHECK((!parser.next_line()));
        CHECK((parser.end_of_file()));

    }
}