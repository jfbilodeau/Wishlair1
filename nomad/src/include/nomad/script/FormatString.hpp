//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_FORMATSTRING_HPP
#define NOMAD_FORMATSTRING_HPP

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
        IdentifierType variable_type;
        const Type* value_type;
        NomadId context_id;
        NomadId variable_id;
        NomadString value;
    };

public:
    explicit FormatString(const NomadString& format_string, NomadId script_id);
    ~FormatString();

    [[nodiscard]] const NomadString& get_format_string() const {
        return m_format_string;
    }

    [[nodiscard]] NomadId get_script_id() const {
        return m_script_id;
    }

    void add_literal(const std::string& literal);
    void add_variable(IdentifierType identifier_type, const Type* value_type, NomadId context_id, NomadId variable_id);

    std::string format(Interpreter* interpreter) const;

private:
    NomadString m_format_string;
    NomadId m_script_id;
    std::vector<Segment> m_segments;
};

} // nomad

#endif //NOMAD_FORMATSTRING_HPP
