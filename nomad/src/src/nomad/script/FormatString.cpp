//
// Created by jfbil on 2023-06-07.
//


#include "nomad/script/FormatString.hpp"

#include "nomad/system/String.hpp"

#include "nomad/script/Runtime.hpp"

namespace nomad {

FormatString::FormatString(const NomadString& format_string, NomadId script_id):
    m_formatString(format_string),
    m_scriptId(script_id)
{}

FormatString::~FormatString() = default;

void FormatString::addLiteral(const NomadString& literal) {
    m_segments.push_back({
    SegmentType::Literal,
    IdentifierType::Unknown,
    nullptr,
    NOMAD_INVALID_ID,
    NOMAD_INVALID_ID,
    literal
    });
}

void FormatString::addVariable(IdentifierType variable_type, const Type* valueType, NomadId contextId, NomadId variableId) {
    m_segments.push_back({
        SegmentType::Variable,
        variable_type,
        valueType,
        contextId,
        variableId,
    });
}

NomadString FormatString::format(Interpreter *interpreter) const {
    NomadString string;

    for (auto const& segment: m_segments) {
        switch (segment.type) {
            case SegmentType::Literal: {
                string += segment.value;
                break;
            }
            case SegmentType::Variable: {
                ScriptValue value;
                auto result = interpreter->getVariableValue(
                        segment.variableType,
                        segment.contextId,
                        segment.variableId,
                        value
                );

                if (!result) {
                    interpreter->fault(
                            "Failed to get variable value for format string segment"
                    );
                }

                string += segment.valueType->to_string(value);

                break;
            }
            default:
                interpreter->fault(
                        "Internal error: Invalid format string segment type: " + toString((int) segment.type)
                );
        }
    }

    return string;
}

} // nomad