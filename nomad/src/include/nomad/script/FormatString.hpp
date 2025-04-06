//
// Created by jfbil on 2023-06-07.
//

#pragma once

#include "nomad/compiler/CompilerException.hpp"
#include "nomad/compiler/Identifier.hpp"

#include "nomad/script/Variable.hpp"

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

// Forward declarations
class Interpreter;

class FormatStringException : public CompilerException {
public:
    explicit FormatStringException(const NomadString& message) : CompilerException(message) {}
};

class FormatString {
    enum class SegmentType {
        Literal = 1,
        Variable,
    };

    struct Segment {
        SegmentType type;
        IdentifierType variableType;
        const Type* valueType;
        NomadId contextId;
        NomadId variableId;
        NomadString value;
    };

public:
    explicit FormatString(const NomadString& format_string, NomadId script_id);
    ~FormatString();

    [[nodiscard]] const NomadString& getFormatString() const {
        return m_formatString;
    }

    [[nodiscard]] NomadId getScriptId() const {
        return m_scriptId;
    }

    void addLiteral(const NomadString& literal);
    void addVariable(IdentifierType identifierType, const Type* valueType, NomadId contextId, NomadId variableId);

    NomadString format(Interpreter* interpreter) const;

private:
    NomadString m_formatString;
    NomadId m_scriptId;
    std::vector<Segment> m_segments;
};

} // nomad

