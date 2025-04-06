//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include "nomad/nomad.hpp"

#include <ostream>
#include <utility>
#include <vector>

namespace nomad {

// Forward declarations
class Runtime;

class Script;

class TokenizerException : public NomadException {
public:
    explicit TokenizerException(const NomadString& message, NomadIndex lineIndex, NomadIndex character);

private:
    NomadIndex m_lineIndex;
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
    NomadString textValue;
    union {
        NomadBoolean booleanValue = NOMAD_FALSE;
        NomadInteger integerValue;
        NomadFloat floatValue;
    };

    Token(TokenType type, NomadString text) :
        type(type),
        textValue(std::move(text))
    {}

    Token(TokenType type, NomadString text, NomadBoolean booleanValue) :
        type(type),
        textValue(std::move(text)),
        booleanValue(booleanValue)
    {}

    Token(TokenType type, NomadString text, NomadInteger integerValue) :
        type(type),
        textValue(std::move(text)),
        integerValue(integerValue)
    {}

    Token(TokenType type, NomadString text, NomadFloat floatValue) :
        type(type),
        textValue(std::move(text)),
        floatValue(floatValue)
    {}

    Token() = default;
};

inline NomadString toString(TokenType type) {
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
    os << toString(type);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(type: " << token.type
       << ", text: " << token.textValue
       << ", boolean: " << token.booleanValue
       << ", integer: " << token.integerValue
       << ", float: " << token.floatValue
       << ")";
    return os;
}

class Tokenizer {
public:
    Tokenizer(Runtime* runtime, const NomadString& source);

//    Tokenizer(Tokenizer&) = delete;

    ~Tokenizer() = default;

    void reset();

    [[nodiscard]] Runtime* getRuntime() const;

    [[nodiscard]] const NomadString& getSource() const;

    [[nodiscard]] bool isEndOfFile() const;

    bool nextLine();

    [[nodiscard]] NomadIndex getLineIndex() const;

    [[nodiscard]] NomadIndex getColumnIndex() const;

    [[nodiscard]] const NomadString& getLine() const;

    [[nodiscard]] NomadIndex getLineCount() const;

    [[nodiscard]] const NomadString& getTokenAt(NomadIndex index) const;

    [[nodiscard]] TokenType getTokenTypeAt(NomadIndex index) const;

    [[nodiscard]] NomadInteger getIntegerTokenAt(NomadIndex index) const;
    [[nodiscard]] NomadFloat getFloatTokenAt(NomadIndex index) const;

    [[nodiscard]] const Token& currentToken() const;

    const Token& nextToken();
    const NomadString& nextIdentifier();
    void nextIdentifier(NomadString& identifier);
    void nextOperator(NomadString& operator_text);
    NomadInteger nextInteger();
    NomadFloat nextFloat();

    [[nodiscard]] NomadIndex getTokenCount() const;

    [[nodiscard]] bool endOfLine() const;

    [[nodiscard]] bool tokenIs(const NomadString& token) const;

    [[nodiscard]] bool tokenIs(TokenType type) const;

    void expect(const NomadString& token);

    void expect(TokenType type);

    void expectEndOfLine();

private:
    void parseLine();

    void skipWhiteSpace(const NomadString& line, int& i);

    [[noreturn]] void throwError(const NomadString& message);

    Runtime* m_runtime;
    NomadString m_source;

    int m_lineIndex = 0;
    std::vector<NomadString> m_lines;
    int m_tokenIndex = 0;
    std::vector<Token> m_tokens;
    bool m_endOfFile = false;
};

} // nomad

