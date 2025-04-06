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
            qi::string(":") |
            qi::string("(") |
            qi::string(")")
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

TokenizerException::TokenizerException(const NomadString& message, NomadIndex lineIndex, NomadIndex character) :
    NomadException(message),
    m_lineIndex(lineIndex),
    m_character(character) {}

Tokenizer::Tokenizer(Runtime* runtime, const NomadString& source) :
    m_runtime(runtime),
    m_source(source) {
    std::stringstream stream(source);

    NomadString line;

    while (std::getline(stream, line)) {
        m_lines.push_back(line);
    }

    m_endOfFile = m_lines.empty();

    if (!isEndOfFile()) {
        parseLine();
    }
}

Runtime* Tokenizer::getRuntime() const {
    return m_runtime;
}

void Tokenizer::reset() {
    m_lineIndex = 0;
    m_tokenIndex = 0;
    m_endOfFile = false;
}

const NomadString& Tokenizer::getSource() const {
    return m_source;
}

//const NomadString& Tokenizer::get_path() const {
//    return m_script->get_path();
//}

bool Tokenizer::isEndOfFile() const {
//    return m_line_index >= (int) m_lines.size();
    return m_endOfFile;
}

bool Tokenizer::nextLine() {
    m_tokens.clear();
    m_tokenIndex = 0;

    do {
        if (m_lineIndex >= m_lines.size()) {
            m_endOfFile = true;

            return false;
        }

        parseLine();

        m_lineIndex++;

        if (getTokenCount() != 0) {
            return true;
        }
    } while (!isEndOfFile());

    return false;
}

bool Tokenizer::tokenIs(const NomadString& token) const {
    return !endOfLine() && currentToken().textValue == token;
}

bool Tokenizer::tokenIs(TokenType type) const {
    if (endOfLine()) {
        return false;
    }

    return currentToken().type == type;
}

void Tokenizer::expect(const NomadString& token) {
    if (!tokenIs(token)) {
        throwError("Expected '" + token + "'");
    }

    nextToken();
}

void Tokenizer::expect(TokenType type) {
    if (!tokenIs(type)) {
        throwError("Expected '" + toString(type) + "'");
    }
}

NomadIndex Tokenizer::getLineIndex() const {
    return m_lineIndex;
}

NomadIndex Tokenizer::getColumnIndex() const {
    return 0; // TODO: Implement
}

const NomadString& Tokenizer::getLine() const {
    return m_lines[m_lineIndex-1];
}

NomadIndex Tokenizer::getLineCount() const {
    return m_lines.size();
}

const NomadString& Tokenizer::getTokenAt(NomadIndex index) const {
    return m_tokens[index].textValue;
}

TokenType Tokenizer::getTokenTypeAt(NomadIndex index) const {
    return m_tokens[index].type;
}

NomadInteger Tokenizer::getIntegerTokenAt(NomadIndex index) const {
    return m_tokens[index].integerValue;
}

NomadFloat Tokenizer::getFloatTokenAt(NomadIndex index) const {
    return m_tokens[index].floatValue;
}


const Token& Tokenizer::currentToken() const {
    return m_tokens[m_tokenIndex];
}

const Token& Tokenizer::nextToken() {
    return m_tokens[m_tokenIndex++];
}

const NomadString& Tokenizer::nextIdentifier() {
    expect(TokenType::Identifier);

    return nextToken().textValue;
}

void Tokenizer::nextIdentifier(NomadString& identifier) {
    expect(TokenType::Identifier);

    identifier = nextToken().textValue;
}

void Tokenizer::nextOperator(NomadString& operator_text) {
    expect(TokenType::Operator);

    operator_text = nextToken().textValue;
}

NomadInteger Tokenizer::nextInteger() {
    expect(TokenType::Integer);

    return nextToken().integerValue;
}

NomadFloat Tokenizer::nextFloat() {
    expect(TokenType::Float);

    return nextToken().floatValue;
}

NomadIndex Tokenizer::getTokenCount() const {
    return m_tokens.size();
}

bool Tokenizer::endOfLine() const {
    return m_tokenIndex >= m_tokens.size();
}

void Tokenizer::expectEndOfLine() {
    if (!endOfLine()) {
        throwError("Expected end of line.");
    }

    nextLine();
}

void Tokenizer::parseLine() {
    auto& line = m_lines[m_lineIndex];

    const LineGrammar<NomadString::iterator> line_grammar;

    auto first = line.begin();

    const auto success = qi::phrase_parse(first, line.end(), line_grammar, line_grammar.skipper, m_tokens);

    if (!success) {
        throwError("Failed to parse line: `" + line + "`");
    }
}

void Tokenizer::skipWhiteSpace(const NomadString& line, int& i) {
    while (i < line.size() && line[i] == ' ') {
        i++;
    }
}

[[noreturn]]
void Tokenizer::throwError(const NomadString& message) {
    throw TokenizerException(message, m_lineIndex, m_tokenIndex);
}

} // nomad