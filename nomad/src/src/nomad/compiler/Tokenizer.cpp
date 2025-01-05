//
// Created by jfbil on 2023-06-04.
//

#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/script/Runtime.hpp"
#include "nomad/script/Script.hpp"

#if defined(NOMAD_DEBUG)
//#define BOOST_SPIRIT_DEBUG
#endif

#define BOOST_NO_CXX98_FUNCTION_BASE
#include "boost/spirit/include/qi.hpp"
#include "boost/phoenix.hpp"

#include <sstream>

BOOST_FUSION_ADAPT_STRUCT(
    nomad::Token,
    (nomad::TokenType, type)
        (nomad::NomadString, text)
        (nomad::NomadBoolean, boolean_value)
        (nomad::NomadInteger, integer_value)
        (nomad::NomadFloat, float_value)
)

namespace nomad {

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

template<typename Iterator>
struct CommentSkipper : qi::grammar<Iterator> {
    CommentSkipper() : CommentSkipper::base_type(skip) {
//        BOOST_SPIRIT_DEBUG_NODE(skip);
    }

    qi::rule<Iterator> skip =
        qi::space |
        ('#' >> *(qi::char_ - qi::eol) >> (qi::eol | qi::eoi));
};

template<typename Iterator>
struct LineGrammar : boost::spirit::qi::grammar<Iterator, std::vector<Token>()> {
    LineGrammar() :
        LineGrammar::base_type(start) {

//        BOOST_SPIRIT_DEBUG_NODE(hex_value);
//        BOOST_SPIRIT_DEBUG_NODE(decimal_value);
//        BOOST_SPIRIT_DEBUG_NODE(value);
//        BOOST_SPIRIT_DEBUG_NODE(escape_sequence);
//        BOOST_SPIRIT_DEBUG_NODE(quoted_content);
//        BOOST_SPIRIT_DEBUG_NODE(format_content);
//        BOOST_SPIRIT_DEBUG_NODE(literal_string);
//        BOOST_SPIRIT_DEBUG_NODE(format_string);
//        BOOST_SPIRIT_DEBUG_NODE(operators);
//        BOOST_SPIRIT_DEBUG_NODE(keyword);
//        BOOST_SPIRIT_DEBUG_NODE(identifier);
//        BOOST_SPIRIT_DEBUG_NODE(basic_token);
//        BOOST_SPIRIT_DEBUG_NODE(token);
//        BOOST_SPIRIT_DEBUG_NODE(start);
//        BOOST_SPIRIT_DEBUG_NODE(skipper);
    }

    CommentSkipper<Iterator> skipper;

//    qi::rule<Iterator, NomadBoolean()>
//        boolean_value = qi::bool_;

    template<typename T>
    struct nomad_real_policies : qi::real_policies<T>
    {
        static bool const expect_dot = true;
    };

    qi::real_parser<NomadFloat, nomad_real_policies<NomadFloat>> float_parser;

    qi::rule<Iterator, NomadFloat()>
        float_value = float_parser;

    qi::rule<Iterator, NomadInteger()>
        hex_value = qi::lit("0x") >> qi::hex,
        decimal_value = qi::int_,
        integer_value = hex_value | decimal_value;

    qi::rule<Iterator, std::string()>
        escape_sequence = qi::lit('\\') >> (qi::char_("\"\\ntr")),
        quoted_content = escape_sequence | (qi::char_ - '"'),
        format_content = escape_sequence | (qi::char_ - '"'),
        literal_string = qi::lexeme['"' >> *quoted_content >> '"'],
        format_string = qi::lexeme["$\"" >> *format_content >> '"'],
        operators = qi::lexeme[
            qi::string("==") |
            qi::string("!=") |
            qi::string(">=") |
            qi::string("<=") |
            qi::string("&&") |
            qi::string("||") |
            qi::string("&") |
            qi::string("^") |
            qi::string("|") |
            qi::string("=") |
            qi::string("+") |
            qi::string("-") |
            qi::string("*") |
            qi::string("/") |
            qi::string("%") |
            qi::string(">") |
            qi::string("<") |
            qi::string(":")
        ],
//        keyword = qi::string("true") | qi::string("false") | qi::string("fun"),
        identifier = (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("._"));

    qi::rule<Iterator, Token()> basic_token = (
//        boolean_value[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Boolean), phoenix::val(""), qi::_1)] |
        float_value[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Float), phoenix::val(""), qi::_1)] |
        integer_value[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Integer), phoenix::val(""), qi::_1)] |
        literal_string[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::String), qi::_1, phoenix::val(0.0))] |
        format_string[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::FormatString), qi::_1, phoenix::val(0.0))] |
        operators[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Operator), qi::_1, phoenix::val(0.0))] |
//        keyword[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Keyword), qi::_1, phoenix::val(0.0))] |
        identifier[qi::_val = phoenix::construct<Token>(phoenix::val(TokenType::Identifier), qi::_1, phoenix::val(0.0))]
    );

    qi::rule<Iterator, Token()> token = basic_token >> *skipper;

    qi::rule<Iterator, std::vector<Token>()> start = *token >> qi::eoi;
};

TokenizerException::TokenizerException(const NomadString& message, NomadIndex line_index, NomadIndex character) :
    NomadException(message),
    m_line_index(line_index),
    m_character(character) {}

Tokenizer::Tokenizer(Runtime* runtime, const NomadString& source) :
    m_runtime(runtime),
    m_source(source) {
    std::stringstream stream(source);

    NomadString line;

    while (std::getline(stream, line)) {
        m_lines.push_back(line);
    }

    m_end_of_file = m_lines.empty();

    if (!end_of_file()) {
        parse_line();
    }
}


Runtime* Tokenizer::get_runtime() const {
    return m_runtime;
}

void Tokenizer::reset() {
    m_line_index = 0;
    m_token_index = 0;
    m_end_of_file = false;
}

const NomadString& Tokenizer::get_source() const {
    return m_source;
}

//const NomadString& Tokenizer::get_path() const {
//    return m_script->get_path();
//}

bool Tokenizer::end_of_file() const {
//    return m_line_index >= (int) m_lines.size();
    return m_end_of_file;
}

bool Tokenizer::next_line() {
    m_tokens.clear();
    m_token_index = 0;

    do {
        if (m_line_index >= m_lines.size()) {
            m_end_of_file = true;

            return false;
        }

        parse_line();

        m_line_index++;

        if (get_token_count() != 0) {
            return true;
        }
    } while (!end_of_file());

    return false;
}

bool Tokenizer::token_is(const NomadString& token) const {
    return !end_of_line() && current_token().text_value == token;
}

bool Tokenizer::token_is(TokenType type) const {
    if (end_of_line()) {
        return false;
    }

    return current_token().type == type;
}

void Tokenizer::expect(const NomadString& token) {
    if (!token_is(token)) {
        throw_error("Expected '" + token + "'");
    }

    next_token();
}

void Tokenizer::expect(TokenType type) {
    if (!token_is(type)) {
        throw_error("Expected '" + to_string(type) + "'");
    }
}

NomadIndex Tokenizer::get_line_index() const {
    return m_line_index;
}

NomadIndex Tokenizer::get_column_index() const {
    return 0; // TODO: Implement
}

const NomadString& Tokenizer::get_line() const {
    return m_lines[m_line_index-1];
}

NomadIndex Tokenizer::get_line_count() const {
    return m_lines.size();
}

const NomadString& Tokenizer::get_token_at(NomadIndex index) const {
    return m_tokens[index].text_value;
}

TokenType Tokenizer::get_token_type_at(NomadIndex index) const {
    return m_tokens[index].type;
}

NomadInteger Tokenizer::get_integer_token_at(NomadIndex index) const {
    return m_tokens[index].integer_value;
}

NomadFloat Tokenizer::get_float_token_at(NomadIndex index) const {
    return m_tokens[index].float_value;
}


const Token& Tokenizer::current_token() const {
    return m_tokens[m_token_index];
}

const Token& Tokenizer::next_token() {
    return m_tokens[m_token_index++];
}

const NomadString& Tokenizer::next_identifier() {
    expect(TokenType::Identifier);

    return next_token().text_value;
}

void Tokenizer::next_identifier(NomadString& identifier) {
    expect(TokenType::Identifier);

    identifier = next_token().text_value;
}

void Tokenizer::next_operator(NomadString& operator_text) {
    expect(TokenType::Operator);

    operator_text = next_token().text_value;
}

NomadInteger Tokenizer::next_integer() {
    expect(TokenType::Integer);

    return next_token().integer_value;
}

NomadFloat Tokenizer::next_float() {
    expect(TokenType::Float);

    return next_token().float_value;
}

NomadIndex Tokenizer::get_token_count() const {
    return m_tokens.size();
}

bool Tokenizer::end_of_line() const {
    return m_token_index >= m_tokens.size();
}

void Tokenizer::expect_end_of_line() {
    if (!end_of_line()) {
        throw_error("Expected end of line.");
    }

    next_line();
}

void Tokenizer::parse_line() {
    auto& line = m_lines[m_line_index];

    const LineGrammar<NomadString::iterator> line_grammar;

    auto first = line.begin();

    const auto success = qi::phrase_parse(first, line.end(), line_grammar, line_grammar.skipper, m_tokens);

    if (!success) {
        throw_error("Failed to parse line: `" + line + "`");
    }
}

void Tokenizer::skip_white_space(const NomadString& line, int& i) {
    while (i < line.size() && line[i] == ' ') {
        i++;
    }
}

[[noreturn]]
void Tokenizer::throw_error(const NomadString& message) {
    throw TokenizerException(message, m_line_index, m_token_index);
}

} // nomad