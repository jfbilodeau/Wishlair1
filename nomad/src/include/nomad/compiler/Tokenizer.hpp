//
// Created by jfbil on 2023-06-04.
//

#ifndef CPP_PARSER_HPP
#define CPP_PARSER_HPP

#include "nomad/Nomad.hpp"

#include <ostream>
#include <utility>
#include <vector>

namespace nomad {

// Forward declarations
class Runtime;

class Script;

class TokenizerException : public NomadException {
public:
    explicit TokenizerException(const NomadString& message, NomadIndex line_index, NomadIndex character);

private:
    NomadIndex m_line_index;
    NomadIndex m_character;
};

enum class TokenType {
    Unknown = 1,
    Operator,
    Keyword,
    Identifier,
    Boolean,
    Integer,
    Float,
    String,
    FormatString,
};

struct Token {
    TokenType type = TokenType::Unknown;
    NomadString text_value;
    union {
        NomadBoolean boolean_value = NOMAD_FALSE;
        NomadInteger integer_value;
        NomadFloat float_value;
    };

    Token(TokenType type, NomadString text) :
        type(type),
        text_value(std::move(text))
    {}

    Token(TokenType type, NomadString text, NomadBoolean boolean_value) :
        type(type),
        text_value(std::move(text)),
        boolean_value(integer_value)
    {}

    Token(TokenType type, NomadString text, NomadInteger integer_value) :
        type(type),
        text_value(std::move(text)),
        integer_value(integer_value)
    {}

    Token(TokenType type, NomadString text, NomadFloat float_value) :
        type(type),
        text_value(std::move(text)),
        float_value(float_value)
    {}

    Token() = default;
};

inline NomadString to_string(TokenType type) {
    switch (type) {
        case TokenType::Unknown:
            return "Unknown";
        case TokenType::Operator:
            return "Operator";
        case TokenType::Keyword:
            return "Keyword";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Boolean:
            return "Boolean";
        case TokenType::Integer:
            return "Integer";
        case TokenType::Float:
            return "Float";
        case TokenType::String:
            return "String";
        case TokenType::FormatString:
            return "FormatString";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    os << to_string(type);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(type: " << token.type
       << ", text: " << token.text_value
       << ", boolean: " << token.boolean_value
       << ", integer: " << token.integer_value
       << ", float: " << token.float_value
       << ")";
    return os;
}

class Tokenizer {
public:
    Tokenizer(Runtime* runtime, const NomadString& source);

//    Tokenizer(Tokenizer&) = delete;

    ~Tokenizer() = default;

    void reset();

    [[nodiscard]] Runtime* get_runtime() const;
//    [[nodiscard]] Script* get_script() const;

    [[nodiscard]] const NomadString& get_source() const;

//    [[nodiscard]] const NomadString& get_path() const;

    [[nodiscard]] bool end_of_file() const;

    bool next_line();

    [[nodiscard]] NomadIndex get_line_index() const;

    [[nodiscard]] NomadIndex get_column_index() const;

    [[nodiscard]] const NomadString& get_line() const;

    [[nodiscard]] NomadIndex get_line_count() const;

    [[nodiscard]] const NomadString& get_token_at(NomadIndex index) const;

    [[nodiscard]] TokenType get_token_type_at(NomadIndex index) const;

    [[nodiscard]] NomadInteger get_integer_token_at(NomadIndex index) const;
    [[nodiscard]] NomadFloat get_float_token_at(NomadIndex index) const;

    [[nodiscard]] const Token& current_token() const;

    const Token& next_token();
    const NomadString& next_identifier();
    void next_identifier(NomadString& identifier);
    void next_operator(NomadString& operator_text);
    NomadInteger next_integer();
    NomadFloat next_float();

    [[nodiscard]] NomadIndex get_token_count() const;

    [[nodiscard]] bool end_of_line() const;

    [[nodiscard]] bool token_is(const NomadString& token) const;

    [[nodiscard]] bool token_is(TokenType type) const;

    void expect(const NomadString& token);

    void expect(TokenType type);

    void expect_end_of_line();

private:
    void parse_line();

    void skip_white_space(const NomadString& line, int& i);

    [[noreturn]] void throw_error(const NomadString& message);

    Runtime* m_runtime;
    NomadString m_source;
//    Script* m_script;

    int m_line_index = 0;
    std::vector<NomadString> m_lines;
    int m_token_index = 0;
    std::vector<Token> m_tokens;
    bool m_end_of_file = false;
};

} // nomad

#endif //CPP_PARSER_HPP
