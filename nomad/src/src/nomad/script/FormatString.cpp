//
// Created by jfbil on 2023-06-07.
//


#include "nomad/script/FormatString.hpp"

#include "nomad/system/String.hpp"

#include "nomad/script/Runtime.hpp"

namespace nomad {

FormatString::FormatString(const NomadString& format_string, NomadId script_id):
    m_format_string(format_string),
    m_script_id(script_id)
{}

FormatString::~FormatString() = default;

void FormatString::add_literal(const NomadString& literal) {
    m_segments.push_back({
    SegmentType::Literal,
    IdentifierType::Unknown,
    nullptr,
    NOMAD_INVALID_ID,
    NOMAD_INVALID_ID,
    literal
    });
}

void FormatString::add_variable(IdentifierType variable_type, const Type* value_type, NomadId context_id, NomadId variable_id) {
    m_segments.push_back({
        SegmentType::Variable,
        variable_type,
        value_type,
        context_id,
        variable_id,
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
                auto result = interpreter->get_variable_value(
                        segment.variable_type,
                        segment.context_id,
                        segment.variable_id,
                        value
                );

                if (!result) {
                    interpreter->fault(
                            "Failed to get variable value for format string segment"
                    );
                }

                string += segment.value_type->to_string(value);

                break;
            }
            default:
                interpreter->fault(
                        "Internal error: Invalid format string segment type: " + to_string((int) segment.type)
                );
        }
    }

    return string;
}

} // nomad